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

#define REQ_THREAD_STACK_SIZE 1024
#define REQ_THREAD_PRIORITY   2

K_THREAD_STACK_DEFINE(req_thread_stack, REQ_THREAD_STACK_SIZE);
static struct k_thread req_thread;
namespace ThingSet::Ip::Zsock {

void req_thread_loop(void *p1, void *p2, void *p3);

ThingSetZephyrSocketServerTransport::ThingSetZephyrSocketServerTransport(struct net_if *iface, const char *ip)
    : _pub_sock(-1), _req_sock(-1), _listener_tid(-1)
{
    net_addr_pton(AF_INET, "255.255.255.255", &_pub_addr.sin_addr);
    _pub_addr.sin_family = AF_INET;
    _pub_addr.sin_port = htons(9002);

    struct net_if_addr *addr = net_if_ipv4_addr_add(iface, &_pub_addr.sin_addr, NET_ADDR_MANUAL, 0);
    __ASSERT(addr != NULL, "Failed to add IP address to interface: %d", errno);

    _pub_sock = zsock_socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    __ASSERT(_pub_sock >= 0, "Failed to create UDP socket: %d", errno);

    int opt_val = 1;
    int ret = zsock_setsockopt(_pub_sock, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof(opt_val));
    ret |= zsock_setsockopt(_pub_sock, SOL_SOCKET, SO_BROADCAST, &opt_val, sizeof(opt_val));
    __ASSERT(ret == 0, "Failed to configure UDP socket: %d", errno);

    net_addr_pton(AF_INET, ip, &_req_addr.sin_addr);
    _req_addr.sin_family = AF_INET;
    _req_addr.sin_port = htons(9001);

    addr = net_if_ipv4_addr_add(iface, &_req_addr.sin_addr, NET_ADDR_MANUAL, 0);
    __ASSERT(addr != NULL, "Failed to add IP address to interface: %d", errno);

    _req_sock = zsock_socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    __ASSERT(_req_sock >= 0, "Failed to create TCP socket: %d", errno);
}

ThingSetZephyrSocketServerTransport::~ThingSetZephyrSocketServerTransport()
{
    zsock_close(_pub_sock);
    zsock_close(_req_sock);
    _pub_sock = -1;
    _req_sock = -1;
}

bool ThingSetZephyrSocketServerTransport::start(std::function<int(DummyEndpoint &, uint8_t *, size_t, uint8_t *, size_t)> callback)
{
    if (zsock_bind(_pub_sock, (struct sockaddr *)&_pub_addr, sizeof(_pub_addr))) {
        return false;
    }

    if (zsock_bind(_req_sock, (struct sockaddr *)&_req_addr, sizeof(_req_addr))) {
        return false;
    }

    _listener_callback = callback;
    _listener_tid = (int)k_thread_create(&req_thread, req_thread_stack, K_THREAD_STACK_SIZEOF(req_thread_stack),
                                          req_thread_loop, this, NULL, NULL, REQ_THREAD_PRIORITY, 0, K_NO_WAIT);

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

    ssize_t sent = zsock_sendto(_pub_sock, buffer, len, 0, (struct sockaddr *)&_pub_addr, sizeof(_pub_addr));
    return sent == (ssize_t)len;
}

int ThingSetZephyrSocketServerTransport::pub_sock() {
    return _pub_sock;
}

int ThingSetZephyrSocketServerTransport::req_sock() {
    return _req_sock;
}

void req_thread_loop(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    ThingSetZephyrSocketServerTransport *transport = static_cast<ThingSetZephyrSocketServerTransport *>(p1);

    struct sockaddr_in client;

    while (1) {
        if (zsock_listen(transport->pub_sock(), 10) != 0) {
            return;
        }

        socklen_t client_length = sizeof(client);

        while (1) {
            char client_addr_str[32];

            int client_sock = zsock_accept(transport->req_sock(), (struct sockaddr *)&client, &client_length);

            if (client_sock < 0) {
                continue;
            }

            zsock_inet_ntop(client.sin_family, &client.sin_addr, client_addr_str, sizeof(client_addr_str));
            printk("Connection from %s\n", client_addr_str);

            while (1) {
                int rx_len = 0;
                uint8_t rx_buf[128] = { 0 };

                rx_len = zsock_recv(client_sock, rx_buf, sizeof(rx_buf), 0);

                if (rx_len < 0) {
                    printk("rx_err: %d\n", rx_len);
                } 
                else if (rx_len == 0) {
                    break;
                }
                else {
                    printk("rx: %s\n", rx_buf);
                }
            }

            zsock_close(client_sock);
        }
    }
}

} // namespace ThingSet::Ip::Zsock