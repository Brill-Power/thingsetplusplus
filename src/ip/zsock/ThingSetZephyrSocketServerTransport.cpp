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

#define TCP_THREAD_STACK_SIZE 1024
#define TCP_THREAD_PRIORITY   2

K_THREAD_STACK_DEFINE(tcp_thread_stack, tcp_THREAD_STACK_SIZE);
static struct k_thread tcp_thread;

namespace ThingSet::Ip::Zsock {

ThingSetZephyrSocketServerTransport::ThingSetZephyrSocketServerTransport(struct net_if *iface, const char *ip)
    : _broadcastSocket(-1), _tcpSocket(-1), _listenerThreadID(-1)
{
    // UDP address configuration - publishes on broadcast address, port 9002
    int ret = net_addr_pton(AF_INET, "255.255.255.255", &_broadcast_addr.sin_addr);
    __ASSERT(ret == 0, "Failed to convert %s to IP address: %d", ip, errno);

    _broadcast_addr.sin_family = AF_INET;
    _broadcast_addr.sin_port = htons(9002);

    _tcp_addr.sin_family = AF_INET;

    // Add broadcast address to specified network interface
    struct net_if_addr *addr = net_if_ipv4_addr_add(iface, &_broadcast_addr.sin_addr, NET_ADDR_MANUAL, 0);
    __ASSERT(addr != NULL, "Failed to add IP address to interface: %d", errno);

    // Create UDP socket for broadcast and configure it accordingly 
    _broadcastSocket = zsock_socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    int opt_val = 1;
    ret |= zsock_setsockopt(_broadcastSocket, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof(opt_val));
    ret |= zsock_setsockopt(_broadcastSocket, SOL_SOCKET, SO_BROADCAST, &opt_val, sizeof(opt_val));
    __ASSERT(_broadcastSocket >= 0, "Failed to create and configure UDP socket: %d", errno);
}

void tcp_thread_loop(void *p1, void *p2, void *p3)
{
    // p1 is a pointer to an instance of ThingSetZephyrSocketClientTransport
    ThingSetZephyrSocketServerTransport *transport = static_cast<ThingSetZephyrSocketServerTransport *>(p1);

    for (;;) {
        uint8_t *buf = transport->get_buffer();

        size_t length = zsock_recv(transport->get_sub_sock(), buf, 1024, 0);

        printk("length: %d\n", length);

        if (length > 0) {
            for (size_t i = 0; i < length; i++) {
                printk("%02X ", buf[i]);
            }

            printk("\n");
        }

        if (length > 4) {
            if (buf[0] != ThingSetRequestType::report) {
                continue;
            }

            size_t actual_length = buf[1] | (buf[2] << 8);

            if (actual_length == (length - 3)) {
                transport->get_callback()(&buf[3], actual_length);
            }
        }
    }

    return;
}

bool ThingSetZephyrSocketServerTransport::listen(std::function<int(uint8_t *, size_t, uint8_t *, size_t)> callback)
{
    struct sockaddr_in _tcp_addr = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = 1,
        .sin_port = htons(9001),
    };

    _tcpSocket = zsock_socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);


    _listenerCallback = callback;
    _listenerThreadID = (int)k_thread_create(&tcp_thread, tcp_thread_stack, K_THREAD_STACK_SIZEOF(tcp_thread_stack),
                                             tcp_thread_loop, this, NULL, NULL, TCP_THREAD_PRIORITY, 0, K_NO_WAIT);

    return true;
}

bool ThingSetZephyrSocketServerTransport::pub_sock_is_bound(void)
{
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);

    if (zsock_getsockname(_broadcastSocket, (struct sockaddr *)&addr, &addr_len) == 0) {
        if (addr.sin_port != 0) {
            return true;
        }
    }

    return false;
}

bool ThingSetZephyrSocketServerTransport::publish(uint8_t *buffer, size_t len)
{
    if (this->pub_sock_is_bound()) {
        ssize_t sent = zsock_sendto(_broadcastSocket, buffer, len, 0, (struct sockaddr *)&_broadcast_addr, sizeof(_broadcast_addr));
        return sent == (ssize_t)len;
    }

    return false;
}

} // namespace ThingSet::Ip::Zsock