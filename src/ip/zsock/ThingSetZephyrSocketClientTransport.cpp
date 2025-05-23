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
    : _socketHandle(-1)
{
    net_addr_pton(AF_INET, ip, &_serverAddress.sin_addr);
    _serverAddress.sin_family = AF_INET;
    _serverAddress.sin_port = htons(9001);

    _socketHandle = zsock_socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    __ASSERT(_socketHandle >= 0, "Failed to create TCP socket: %d", errno);
}

ThingSetZephyrSocketClientTransport::~ThingSetZephyrSocketClientTransport()
{
    zsock_close(_socketHandle);
    _socketHandle = -1;
}

bool ThingSetZephyrSocketClientTransport::connect()
{
    return zsock_connect(_socketHandle, (struct sockaddr *)&_serverAddress, sizeof(_serverAddress)) == 0;
}

int ThingSetZephyrSocketClientTransport::read(uint8_t *buffer, size_t len)
{
    return zsock_recv(_socketHandle, buffer, len, 0);
}

bool ThingSetZephyrSocketClientTransport::write(uint8_t *buffer, size_t len)
{
    return zsock_send(_socketHandle, buffer, len, 0) == (ssize_t)len;
}

} // namespace ThingSet::Ip::Zsock