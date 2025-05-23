/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "thingset++/ip/zsock/ThingSetZephyrSocketServerTransport.hpp"
#include <assert.h>
#include <zephyr/kernel.h>
#include <zephyr/posix/poll.h>
#include <array>
#include <iostream>

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

ThingSetZephyrSocketServerTransport::ThingSetZephyrSocketServerTransport(net_if *iface)
    : _publishSocketHandle(-1), _listenSocketHandle(-1), _acceptorThreadId(nullptr), _handlerThreadId(nullptr)
{
    // find IP address associated with interface
    net_if_ipv4 *ipConfig;
    int ret = net_if_config_ipv4_get(iface, &ipConfig);
    __ASSERT(ret == 0, "Failed to get interface IP information: %d", ret);

    // calculate broadcast address
    _broadcastAddress = {
        .sin_family = AF_INET,
        .sin_port = htons(9002),
        .sin_addr = {
            .s_addr = ipConfig->unicast->ipv4.address.in_addr.s_addr | (~ipConfig->unicast->netmask.s_addr)
        },
    };

    // local address of publish socket
    _publishAddress.sin_addr = ipConfig->unicast->ipv4.address.in_addr;
    _publishAddress.sin_family = AF_INET;
    _publishAddress.sin_port = 0;

    _publishSocketHandle = zsock_socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    __ASSERT(_publishSocketHandle >= 0, "Failed to create UDP socket: %d", errno);

    int opt_val = 1;
    ret = zsock_setsockopt(_publishSocketHandle, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof(opt_val));
    __ASSERT(ret == 0, "Failed to configure UDP socket: %d", errno);

    // local address of listener
    _listenAddress.sin_addr = ipConfig->unicast->ipv4.address.in_addr;
    _listenAddress.sin_family = AF_INET;
    _listenAddress.sin_port = htons(9001);

    _listenSocketHandle = zsock_socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    __ASSERT(_listenSocketHandle >= 0, "Failed to create TCP socket: %d", errno);
}

ThingSetZephyrSocketServerTransport::~ThingSetZephyrSocketServerTransport()
{
    zsock_close(_publishSocketHandle);
    zsock_close(_listenSocketHandle);
    _publishSocketHandle = -1;
    _listenSocketHandle = -1;
}

bool ThingSetZephyrSocketServerTransport::listen(std::function<int(const SocketEndpoint &, uint8_t *, size_t, uint8_t *, size_t)> callback)
{
    if (zsock_bind(_publishSocketHandle, (struct sockaddr *)&_publishAddress, sizeof(_publishAddress))) {
        return false;
    }

    if (zsock_bind(_listenSocketHandle, (struct sockaddr *)&_listenAddress, sizeof(_listenAddress))) {
        return false;
    }

    _callback = callback;
    _handlerThreadId = k_thread_create(&handler_thread, handler_thread_stack, K_THREAD_STACK_SIZEOF(handler_thread_stack),
                                   runHandler, this, NULL, NULL, HANDLER_THREAD_PRIORITY, 0, K_NO_WAIT);

    _acceptorThreadId = k_thread_create(&accept_thread, accept_thread_stack, K_THREAD_STACK_SIZEOF(accept_thread_stack),
                                  runAcceptor, this, NULL, NULL, ACCEPT_THREAD_PRIORITY, 0, K_NO_WAIT);

    return true;
}

bool ThingSetZephyrSocketServerTransport::publish(uint8_t *buffer, size_t len)
{
    sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    if (zsock_getsockname(_publishSocketHandle, (struct sockaddr *)&addr, &addr_len) != 0) {
        return false;
    }

    // sin_port will be non-zero if currently bound, socket must be bound to send
    if (addr.sin_port == 0) {
        return false;
    }

    ssize_t sent = zsock_sendto(_publishSocketHandle, buffer, len, 0, (struct sockaddr *)&_broadcastAddress, sizeof(_broadcastAddress));
    return sent == (ssize_t)len;
}

void ThingSetZephyrSocketServerTransport::runAcceptor(void *p1, void *, void *)
{
    ThingSetZephyrSocketServerTransport *transport = static_cast<ThingSetZephyrSocketServerTransport *>(p1);
    transport->runAcceptor();
}

void ThingSetZephyrSocketServerTransport::runAcceptor()
{
    if (zsock_listen(_listenSocketHandle, MAX_CLIENTS) != 0) {
        printk("Failed to begin listening: %d\n", errno);
        return;
    }

    while (true) {
        sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        char client_addr_str[INET_ADDRSTRLEN];

        int client_sock = zsock_accept(_listenSocketHandle, (sockaddr *)&client_addr, &client_addr_len);
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

        SocketEndpoint addr;
        socklen_t len = sizeof(addr);
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

                    zsock_getsockname(client_sock, (sockaddr *)&addr, &len);
                    zsock_inet_ntop(AF_INET, &addr.sin_addr, ip, sizeof(ip));

                    //printk("Closing connection from %s\n", ip);
                    std::cout << "Closing connection from " << addr << std::endl;

                    zsock_close(client_sock);
                    sockfd_tcp[i].fd = -1;
                }
                else {
                    zsock_getsockname(client_sock, (sockaddr *)&addr, &len);
                    auto tx_len = _callback(addr, rx_buf, rx_len, tx_buf, sizeof(tx_buf));
                    zsock_send(client_sock, tx_buf, tx_len, 0);
                }
            }
        }
    }
}

} // namespace ThingSet::Ip::Zsock