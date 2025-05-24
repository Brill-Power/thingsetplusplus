/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "thingset++/ThingSetClientTransport.hpp"
#include <cstdint>
#include <cstdio>
#include <string>
#ifdef __ZEPHYR__
#include <zephyr/kernel.h>
#include <zephyr/net/net_if.h>
#include <zephyr/net/socket.h>
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#endif // __ZEPHYR__

namespace ThingSet::Ip::Sockets {

/// @brief Client transport using POSIX sockets.
class ThingSetSocketClientTransport : public ThingSetClientTransport
{
    private:
        struct sockaddr_in _serverAddress;
        int _socketHandle;

    public:
        ThingSetSocketClientTransport(const std::string &ip);
        ~ThingSetSocketClientTransport();

        bool connect() override;
        int read(uint8_t *buffer, size_t len) override;
        bool write(uint8_t *buffer, size_t len) override;
};

} // namespace ThingSet::Ip::Sockets
