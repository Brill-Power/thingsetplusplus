/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "thingset++/ip/sockets/SocketEndpoint.hpp"
#include "thingset++/ThingSetSubscriptionTransport.hpp"
#include <cstdint>
#include <cstdio>
#ifdef __ZEPHYR__
#include <zephyr/net/net_if.h>
#else
#include <thread>
#endif // __ZEPHYR__

namespace ThingSet::Ip::Sockets {

/// @brief Subscription transport for sockets.
class _ThingSetSocketSubscriptionTransport : public ThingSetMultiFrameSubscriptionTransport<SocketEndpoint>
{
private:
    sockaddr_in _listenAddress;
    int _listenSocketHandle;
    std::function<void(const SocketEndpoint &, ThingSetBinaryDecoder &)> _callback;

protected:
    _ThingSetSocketSubscriptionTransport();
    ~_ThingSetSocketSubscriptionTransport();

    bool subscribe(std::function<void(const SocketEndpoint &, ThingSetBinaryDecoder &)> callback) override;

    void runListener();
    virtual void startThread() = 0;
};

class ThingSetSocketSubscriptionTransport : public _ThingSetSocketSubscriptionTransport
{
#ifdef __ZEPHYR__
private:
    k_tid_t _listenerThreadId;

    using _ThingSetSocketSubscriptionTransport::runListener;
    static void runListener(void *p1, void *, void *);

protected:
    void startThread() override;
#else
private:
    std::thread _listenerThread;

protected:
    void startThread() override;
#endif // __ZEPHYR__
};

} // namespace ThingSet::Ip::Sockets