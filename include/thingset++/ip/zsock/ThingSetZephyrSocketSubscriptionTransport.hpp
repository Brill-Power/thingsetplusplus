/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "thingset++/ThingSetSubscriptionTransport.hpp"
#include <cstdint>
#include <cstdio>
#include <zephyr/net/net_if.h>

namespace ThingSet::Ip::Zsock {

struct DummyEndpoint {};

class ThingSetZephyrSocketSubscriptionTransport : public ThingSetSubscriptionTransport<DummyEndpoint>
{
private:
    struct sockaddr_in _udp_addr;
    int _sub_sock;
    int _listener_tid;
    uint8_t _buffer[1024];
    std::function<void(DummyEndpoint &, uint8_t *, size_t)> _listener_callback;

public:
    ThingSetZephyrSocketSubscriptionTransport(struct net_if *iface);
    ~ThingSetZephyrSocketSubscriptionTransport();

    bool listen() override;
    void subscribe(std::function<void(DummyEndpoint &, uint8_t *, size_t)> callback) override;

    uint8_t *get_buffer();
    std::function<void(DummyEndpoint &, uint8_t *, size_t)> get_callback();
    int get_sub_sock();
};

} // namespace ThingSet::Ip::Zsock