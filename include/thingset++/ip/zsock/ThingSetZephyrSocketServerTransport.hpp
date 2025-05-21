/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "thingset++/ip/udp/ThingSetUdpServerTransport.hpp"
#include <cstdint>
#include <cstdio>
#include <zephyr/kernel.h>
#include <zephyr/net/net_if.h>
#include <zephyr/net/socket.h>

namespace ThingSet::Ip::Zsock {

using ThingSet::Ip::Udp::ThingSetUdpServerTransport;
struct DummyServerEndpoint {};

class ThingSetZephyrSocketServerTransport : public ThingSetUdpServerTransport<DummyServerEndpoint>
{
    private:
        struct sockaddr_in _pub_addr;
        struct sockaddr_in _req_addr;
        int _pub_sock;
        int _req_sock;
        k_tid_t _accept_tid;
        k_tid_t _handler_tid;
        std::function<int(const DummyServerEndpoint &, uint8_t *, size_t, uint8_t *, size_t)> _handler_callback;

    public:
        ThingSetZephyrSocketServerTransport(struct net_if *iface, const char *ip);
        ~ThingSetZephyrSocketServerTransport();

        bool listen(std::function<int(const DummyServerEndpoint &, uint8_t *, size_t, uint8_t *, size_t)> callback) override;
        bool publish(uint8_t *buffer, size_t len) override;

        int pub_sock(void);
        int req_sock(void);
        std::function<int(const DummyServerEndpoint &, uint8_t *, size_t, uint8_t *, size_t)> callback(void);

};

} // namespace ThingSet::Ip::Zsock
