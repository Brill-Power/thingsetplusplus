/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "thingset++/ip/sockets/ThingSetSocketClientTransport.hpp"
#include "thingset++/ThingSetStatus.hpp"
#include <assert.h>
#ifdef __ZEPHYR__
#include "thingset++/ip/sockets/ZephyrStubs.h"
#include <zephyr/kernel.h>
#include <zephyr/net/net_if.h>
#include <zephyr/net/net_ip.h>
#include <zephyr/net/socket.h>
#else
#include <unistd.h>
#include <stdexcept>
#define __ASSERT(test, fmt, ...) { if (!(test)) { throw std::invalid_argument(fmt); } }
#endif // __ZEPHYR__

namespace ThingSet::Ip::Sockets {

ThingSetSocketClientTransport::ThingSetSocketClientTransport(const std::string &ip)
    : _socketHandle(-1)
{
    int ret = inet_pton(AF_INET, ip.c_str(), &_serverAddress.sin_addr);
    __ASSERT(ret == 1, "Failed to parse supplied IP address %s: %d", ip.c_str(), ret);
    _serverAddress.sin_family = AF_INET;
    _serverAddress.sin_port = htons(9001);

    _socketHandle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    __ASSERT(_socketHandle >= 0, "Failed to create client socket: %d", errno);
}

ThingSetSocketClientTransport::~ThingSetSocketClientTransport()
{
    close(_socketHandle);
    _socketHandle = -1;
}

bool ThingSetSocketClientTransport::connect()
{
    return ::connect(_socketHandle, (struct sockaddr *)&_serverAddress, sizeof(_serverAddress)) == 0;
}

int ThingSetSocketClientTransport::read(uint8_t *buffer, size_t len)
{
    return recv(_socketHandle, buffer, len, 0);
}

bool ThingSetSocketClientTransport::write(uint8_t *buffer, size_t len)
{
    return send(_socketHandle, buffer, len, 0) == (ssize_t)len;
}

} // namespace ThingSet::Ip::Sockets