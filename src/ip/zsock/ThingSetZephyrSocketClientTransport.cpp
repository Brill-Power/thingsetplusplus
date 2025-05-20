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

namespace ThingSet::Ip::Zsock {

ThingSetZephyrSocketClientTransport::ThingSetZephyrSocketClientTransport(struct net_if *iface, const char *ip)
    : _req_sock(-1)
{
    net_addr_pton(AF_INET, ip, &_req_addr.sin_addr);
    _req_addr.sin_family = AF_INET;
    _req_addr.sin_port = htons(9001);
    
    _req_sock = zsock_socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    __ASSERT(_req_sock >= 0, "Failed to create TCP socket: %d", errno);
}

ThingSetZephyrSocketClientTransport::~ThingSetZephyrSocketClientTransport()
{
    zsock_close(_req_sock);
    _req_sock = -1;
}

bool ThingSetZephyrSocketClientTransport::connect() 
{
    return zsock_connect(_req_sock, (struct sockaddr *)&_req_addr, sizeof(_req_addr)) == 0;
}

int ThingSetZephyrSocketClientTransport::read(uint8_t *buffer, size_t len)
{
    return zsock_recv(_req_sock, buffer, len, 0);
}

bool ThingSetZephyrSocketClientTransport::write(uint8_t *buffer, size_t len)
{
    return zsock_send(_req_sock, buffer, len, 0) == (ssize_t)len;
}

} // namespace ThingSet::Ip::Zsock