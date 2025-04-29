/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "thingset++/ThingSetClientTransport.hpp"
#include <cstdint>
#include <cstdio>
#include <zephyr/kernel.h>
#include <zephyr/net/net_if.h>
#include <zephyr/net/socket.h>

namespace ThingSet::Ip::Zsock {

class ThingSetZephyrSocketClientTransport : public ThingSetClientTransport
{
    private:
        struct sockaddr_in _req_addr;
        int _req_sock;

    public:
        ThingSetZephyrSocketClientTransport(struct net_if *iface, const char *ip);
        ~ThingSetZephyrSocketClientTransport();

        bool connect() override;
        int read(uint8_t *buffer, size_t len) override;
        bool write(uint8_t *buffer, size_t len) override;
};

} // namespace ThingSet::Ip::Zsock
