/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "thingset++/ip/zsock/SocketEndpoint.hpp"
#include "thingset++/ThingSetSubscriptionTransport.hpp"
#include <cstdint>
#include <cstdio>
#include <zephyr/net/net_if.h>

namespace ThingSet::Ip::Zsock {

/// @brief Subscription transport for Zephyr sockets.
class ThingSetZephyrSocketSubscriptionTransport : public ThingSetSubscriptionTransport<SocketEndpoint>
{
private:
    sockaddr_in _listenAddress;
    int _listenSocketHandle;
    k_tid_t _listenerThreadId;
    uint8_t _buffer[1024];
    std::function<void(const SocketEndpoint &, ThingSetBinaryDecoder &)> _callback;

public:
    ThingSetZephyrSocketSubscriptionTransport(net_if *iface);
    ~ThingSetZephyrSocketSubscriptionTransport();

    bool subscribe(std::function<void(const SocketEndpoint &, ThingSetBinaryDecoder &)> callback) override;

private:
    static void runSubscriber(void *p1, void *, void *);
    void runSubscriber();
};

} // namespace ThingSet::Ip::Zsock