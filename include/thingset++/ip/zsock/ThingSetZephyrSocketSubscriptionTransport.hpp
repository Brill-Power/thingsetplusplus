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
    k_tid_t _listener_tid;
    uint8_t _buffer[1024];
    std::function<void(const DummyEndpoint &, ThingSetBinaryDecoder &)> _listener_callback;

public:
    ThingSetZephyrSocketSubscriptionTransport(struct net_if *iface, const char *ip);
    ~ThingSetZephyrSocketSubscriptionTransport();

    bool subscribe(std::function<void(const DummyEndpoint &, ThingSetBinaryDecoder &)> callback) override;

    uint8_t *get_buffer();
    std::function<void(const DummyEndpoint &, ThingSetBinaryDecoder &)> get_callback();
    int get_sub_sock();
};

} // namespace ThingSet::Ip::Zsock