/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "thingset++/ThingSetClientTransport.hpp"
#include <asio/awaitable.hpp>
#include <asio/ip/tcp.hpp>
#include <asio/ip/udp.hpp>
#include <cstdint>
#include <cstdio>

namespace ThingSet::Async {

class ThingSetAsyncSocketClientTransport : public ThingSetClientTransport
{
private:
    asio::io_context _ioContext;
    asio::ip::tcp::socket _requestResponseSocket;
    asio::ip::udp::socket _subscribeSocket;
    asio::ip::tcp::endpoint &_endpoint;
    uint8_t _buffer[1024];

public:
    ThingSetAsyncSocketClientTransport(asio::ip::tcp::endpoint &endpoint);

    asio::io_context &getContext();

    bool connect() override;
    int read(uint8_t *buffer, size_t len) override;
    bool write(uint8_t *buffer, size_t len) override;
    void subscribe(std::function<void(uint8_t *, size_t)> callback) override;

private:
    asio::awaitable<void> listener(std::function<void(uint8_t *, size_t)> callback);
};

} // namespace ThingSet::Async