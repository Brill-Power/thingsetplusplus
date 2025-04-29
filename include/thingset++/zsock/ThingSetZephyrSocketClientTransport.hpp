/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "thingset++/ThingSetClientTransport.hpp"
#include <cstdint>
#include <cstdio>
#include <zephyr/net/socket.h>

namespace ThingSet::Zsock {

class ThingSetZephyrSocketClientTransport
{
    private:
        struct sockaddr_in _servaddr;
        struct sockaddr_in _cli;

        int _sock;
        int _opt;
        int _optlen;

    public:
        ThingSetZephyrSocketClientTransport(const char *ip, uint16_t port);

        // bool connect() override;
        // int read(uint8_t *buffer, size_t len) override;
        // bool write(uint8_t *buffer, size_t len) override;
        // void subscribe(std::function<void(uint8_t *, size_t)> callback) override;
};

} // namespace ThingSet::Zsock
