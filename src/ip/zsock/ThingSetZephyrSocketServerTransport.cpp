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
    : _pub_sock(-1), _tcpSocket(-1), _listenerThreadID(-1)
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
}

bool ThingSetZephyrSocketServerTransport::start(std::function<int(uint8_t *, size_t, uint8_t *, size_t)> callback)
{
    // struct sockaddr_in _tcp_addr = {
    //     .sin_family = AF_INET,
    //     .sin_addr.s_addr = 1,
    //     .sin_port = htons(9001),
    // };

    // _tcpSocket = zsock_socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);


    // _listenerCallback = callback;
    // _listenerThreadID = (int)k_thread_create(&tcp_thread, tcp_thread_stack, K_THREAD_STACK_SIZEOF(tcp_thread_stack),
    //                                          tcp_thread_loop, this, NULL, NULL, TCP_THREAD_PRIORITY, 0, K_NO_WAIT);

    // return zsock_bind(_sub_sock, (struct sockaddr *)&_udp_addr, sizeof(_udp_addr)) == 0;

    if (zsock_bind(_pub_sock, (struct sockaddr *)&_pub_addr, sizeof(_pub_addr))) {
        return false;
    }

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

} // namespace ThingSet::Ip::Zsock