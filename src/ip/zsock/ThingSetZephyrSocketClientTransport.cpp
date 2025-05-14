/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "thingset++/ip/zsock/ThingSetZephyrSocketClientTransport.hpp"
#include "thingset++/ThingSetStatus.hpp"
#include <assert.h>
#include <zephyr/kernel.h>
#include <zephyr/net/net_if.h>
#include <zephyr/net/net_ip.h>
#include <zephyr/net/socket.h>

#define UDP_THREAD_STACK_SIZE 1024
#define UDP_THREAD_PRIORITY   2

K_THREAD_STACK_DEFINE(udp_thread_stack, UDP_THREAD_STACK_SIZE);
static struct k_thread udp_thread;

namespace ThingSet::Ip::Zsock {

ThingSetZephyrSocketClientTransport::ThingSetZephyrSocketClientTransport(struct net_if *iface, const char *ip,
                                                                         uint16_t port)
    : _requestResponseSocket(-1)
{
    // TCP address configuration - uses user-supplied IP address and port
    int ret = net_addr_pton(AF_INET, ip, &_nodeaddr.sin_addr);
    _nodeaddr.sin_family = AF_INET;
    _nodeaddr.sin_port = htons(port);

    // Add IP address to specified network interface
    struct net_if_addr *addr = net_if_ipv4_addr_add(iface, &_nodeaddr.sin_addr, NET_ADDR_MANUAL, 0);

    _requestResponseSocket = zsock_socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    __ASSERT(ret == 0, "Failed to convert %s to IP address: %d", ip, errno);
    __ASSERT(addr != NULL, "Failed to add IP address to interface: %d", errno);
    __ASSERT(_requestResponseSocket >= 0, "Failed to create TCP socket: %d", errno);
}

bool ThingSetZephyrSocketClientTransport::set_client_connection_params(const char *ip, uint16_t port)
{
    _clientaddr.sin_family = AF_INET;
    _clientaddr.sin_port = htons(port);

    return net_addr_pton(AF_INET, ip, &_clientaddr.sin_addr) == 0;
}

bool ThingSetZephyrSocketClientTransport::connect()
{
    return zsock_connect(_requestResponseSocket, (struct sockaddr *)&_clientaddr, sizeof(_clientaddr)) == 0;
}

int ThingSetZephyrSocketClientTransport::read(uint8_t *buffer, size_t len)
{
    return zsock_recv(_requestResponseSocket, buffer, len, 0);
}

bool ThingSetZephyrSocketClientTransport::write(uint8_t *buffer, size_t len)
{
    return zsock_send(_requestResponseSocket, buffer, len, 0) == (ssize_t)len;
}

} // namespace ThingSet::Ip::Zsock