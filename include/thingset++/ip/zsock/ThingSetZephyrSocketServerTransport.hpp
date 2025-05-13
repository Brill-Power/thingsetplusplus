/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "thingset++/ThingSetServerTransport.hpp"
#include <cstdint>
#include <cstdio>
#include <zephyr/kernel.h>
#include <zephyr/net/net_if.h>
#include <zephyr/net/socket.h>

namespace ThingSet::Ip::Zsock {

class ThingSetZephyrSocketServerTransport : public ThingSetServerTransport
{
    private:
        struct sockaddr_in _broadcast_addr;
        struct sockaddr_in _tcp_addr;
        int _broadcastSocket;
        int _tcpSocket;

        int _listenerThreadID;
        std::function<int(uint8_t *, size_t, uint8_t *, size_t)> _listenerCallback;

    public:
        ThingSetZephyrSocketServerTransport(struct net_if *iface, const char *ip);

        bool listen(std::function<int(uint8_t *, size_t, uint8_t *, size_t)> callback) override;
        bool publish(uint8_t *buffer, size_t len) override;

    private:
        bool pub_sock_is_bound(void);
};

} // namespace ThingSet::Ip::Zsock
