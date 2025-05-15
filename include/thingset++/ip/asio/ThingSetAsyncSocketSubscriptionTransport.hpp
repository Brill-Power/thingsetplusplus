/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "thingset++/ThingSetSubscriptionTransport.hpp"
#include <asio/awaitable.hpp>
#include <asio/ip/udp.hpp>
#include <cstdint>
#include <cstdio>

namespace ThingSet::Ip::Async {

class ThingSetAsyncSocketSubscriptionTransport : public ThingSetSubscriptionTransport<asio::ip::udp::endpoint>
{
private:
    asio::io_context &_ioContext;
    asio::ip::udp::socket _subscribeSocket;
    uint8_t _buffer[1024];

public:
    ThingSetAsyncSocketSubscriptionTransport(asio::io_context &ioContext);
    ~ThingSetAsyncSocketSubscriptionTransport();

    bool connect() override;
    void subscribe(std::function<void(const asio::ip::udp::endpoint &, ThingSetBinaryDecoder &)> callback) override;

private:
    asio::awaitable<void> listener(std::function<void(const asio::ip::udp::endpoint &, ThingSetBinaryDecoder &)> callback);
};

} // namespace ThingSet::Ip::Async