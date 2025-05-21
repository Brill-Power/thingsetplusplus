/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "thingset++/ip/zsock/ThingSetZephyrSocketServerTransport.hpp"
#include <assert.h>
#include <zephyr/kernel.h>
#include <zephyr/net/net_if.h>
#include <zephyr/net/net_ip.h>
#include <zephyr/net/socket.h>
#include <zephyr/posix/poll.h>
#include <array>

#define ACCEPT_THREAD_STACK_SIZE  1024
#define ACCEPT_THREAD_PRIORITY    3
#define HANDLER_THREAD_STACK_SIZE 4096
#define HANDLER_THREAD_PRIORITY   2
#define MAX_CLIENTS               8

K_THREAD_STACK_DEFINE(accept_thread_stack, ACCEPT_THREAD_STACK_SIZE);
static struct k_thread accept_thread;

K_THREAD_STACK_DEFINE(handler_thread_stack, HANDLER_THREAD_STACK_SIZE);
static struct k_thread handler_thread;

struct PollDescriptor : public pollfd {
public:
    PollDescriptor()
    {
        fd = -1;
        events = ZSOCK_POLLIN;
    }
};

static std::array<PollDescriptor, MAX_CLIENTS> sockfd_tcp;

namespace ThingSet::Ip::Zsock {

ThingSetZephyrSocketServerTransport::ThingSetZephyrSocketServerTransport(struct net_if *iface, const char *ip)
    : _pub_sock(-1), _req_sock(-1), _accept_tid(nullptr), _handler_tid(nullptr)
{
    net_addr_pton(AF_INET, ip, &_pub_addr.sin_addr);
    _pub_addr.sin_family = AF_INET;
    _pub_addr.sin_port = htons(9002);

    _pub_sock = zsock_socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    __ASSERT(_pub_sock >= 0, "Failed to create UDP socket: %d", errno);

    int opt_val = 1;
    int ret = zsock_setsockopt(_pub_sock, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof(opt_val));
    __ASSERT(ret == 0, "Failed to configure UDP socket: %d", errno);

    net_addr_pton(AF_INET, ip, &_req_addr.sin_addr);
    _req_addr.sin_family = AF_INET;
    _req_addr.sin_port = htons(9001);

    _req_sock = zsock_socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    __ASSERT(_req_sock >= 0, "Failed to create TCP socket: %d", errno);

    struct net_if_addr *addr = net_if_ipv4_addr_add(iface, &_req_addr.sin_addr, NET_ADDR_MANUAL, 0);
    __ASSERT(addr != NULL, "Failed to add IP address to interface: %d", errno);

    struct sockaddr_in subnet;
    // get from config?
    net_addr_pton(AF_INET, "255.255.255.0", &subnet.sin_addr);
    net_if_ipv4_set_netmask_by_addr(iface, &_req_addr.sin_addr, &subnet.sin_addr);
}

ThingSetZephyrSocketServerTransport::~ThingSetZephyrSocketServerTransport()
{
    zsock_close(_pub_sock);
    zsock_close(_req_sock);
    _pub_sock = -1;
    _req_sock = -1;
}

bool ThingSetZephyrSocketServerTransport::listen(std::function<int(const DummyServerEndpoint &, uint8_t *, size_t, uint8_t *, size_t)> callback)
{
    if (zsock_bind(_pub_sock, (struct sockaddr *)&_pub_addr, sizeof(_pub_addr))) {
        return false;
    }

    if (zsock_bind(_req_sock, (struct sockaddr *)&_req_addr, sizeof(_req_addr))) {
        return false;
    }

    _callback = callback;
    _handler_tid = k_thread_create(&handler_thread, handler_thread_stack, K_THREAD_STACK_SIZEOF(handler_thread_stack),
                                   runHandler, this, NULL, NULL, HANDLER_THREAD_PRIORITY, 0, K_NO_WAIT);

    _accept_tid = k_thread_create(&accept_thread, accept_thread_stack, K_THREAD_STACK_SIZEOF(accept_thread_stack),
                                  runAcceptor, this, NULL, NULL, ACCEPT_THREAD_PRIORITY, 0, K_NO_WAIT);

    return true;
}

bool ThingSetZephyrSocketServerTransport::publish(uint8_t *buffer, size_t len)
{
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);

    if (zsock_getsockname(_pub_sock, (struct sockaddr *)&addr, &addr_len) != 0) {
        return false;
    }

    // sin_port will be non-zero if currently bound, socket must be bound to send
    if (addr.sin_port == 0) {
        return false;
    }

    struct sockaddr_in broadcast_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(9002),
    };

    // TODO: calculate or get from config
    net_addr_pton(AF_INET, "192.0.2.255", &broadcast_addr.sin_addr);

    ssize_t sent = zsock_sendto(_pub_sock, buffer, len, 0, (struct sockaddr *)&broadcast_addr, sizeof(broadcast_addr));
    return sent == (ssize_t)len;
}

void ThingSetZephyrSocketServerTransport::runAcceptor(void *p1, void *, void *)
{
    ThingSetZephyrSocketServerTransport *transport = static_cast<ThingSetZephyrSocketServerTransport *>(p1);
    transport->runAcceptor();
}

void ThingSetZephyrSocketServerTransport::runAcceptor()
{
    if (zsock_listen(_req_sock, MAX_CLIENTS) != 0) {
        printk("Failed to begin listening: %d\n", errno);
        return;
    }

    while (true) {
        sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        char client_addr_str[INET_ADDRSTRLEN];

        int client_sock = zsock_accept(_req_sock, (sockaddr *)&client_addr, &client_addr_len);
        if (client_sock < 0) {
            printk("Accept failed: %d\n", errno);
            continue;
        }

        zsock_inet_ntop(client_addr.sin_family, &client_addr.sin_addr, client_addr_str,
                        sizeof(client_addr_str));
        printk("Connection from %s\n", client_addr_str);

        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (sockfd_tcp[i].fd == -1) {
                sockfd_tcp[i].fd = client_sock;
                printk("Assigned slot %d\n", i);
                break;
            }
        }
    }
}

void ThingSetZephyrSocketServerTransport::runHandler(void *p1, void *, void *)
{
    ThingSetZephyrSocketServerTransport *transport = static_cast<ThingSetZephyrSocketServerTransport *>(p1);
    transport->runHandler();
}

void ThingSetZephyrSocketServerTransport::runHandler()
{
    while (true) {
        int ret = zsock_poll(sockfd_tcp.data(), sockfd_tcp.size(), 10);

        if (ret < 0) {
            printk("Polling error: %d\n", errno);
        }
        else if (ret == 0) {
            continue;
        }

        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (sockfd_tcp[i].revents & ZSOCK_POLLIN) {
                int client_sock = sockfd_tcp[i].fd;
                int rx_len = 0;
                uint8_t rx_buf[1024];
                uint8_t tx_buf[1024];

                rx_len = zsock_recv(client_sock, rx_buf, sizeof(rx_buf), 0);

                if (rx_len < 0) {
                    printk("Receive error: %d\n", errno);
                }
                else if (rx_len == 0) {
                    char ip[INET_ADDRSTRLEN];
                    struct sockaddr_in addr;
                    socklen_t len = sizeof(addr);

                    zsock_getsockname(client_sock, (struct sockaddr *)&addr, &len);
                    zsock_inet_ntop(AF_INET, &addr.sin_addr, ip, sizeof(ip));

                    printk("Closing connection from %s\n", ip);

                    zsock_close(client_sock);
                    sockfd_tcp[i].fd = -1;
                }
                else {
                    DummyServerEndpoint E;
                    auto tx_len = _callback(E, rx_buf, rx_len, tx_buf, sizeof(tx_buf));
                    zsock_send(client_sock, tx_buf, tx_len, 0);
                }
            }
        }
    }
}

} // namespace ThingSet::Ip::Zsock