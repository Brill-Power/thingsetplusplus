/*
 * Copyright (c) 2024 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */
#pragma once

#include "ThingSetClientTransport.hpp"
#include <asio/awaitable.hpp>
#include <asio/ip/tcp.hpp>
#include <asio/ip/udp.hpp>
#include <cstdint>
#include <cstdio>

namespace ThingSet {

class ThingSetAsyncSocketClientTransport : public ThingSetClientTransport
{
private:
    asio::ip::tcp::endpoint &_endpoint;
    asio::io_context _ioContext;
    asio::ip::tcp::socket _requestResponseSocket;
    asio::ip::udp::socket _subscribeSocket;
    uint8_t _buffer[1024];

public:
    ThingSetAsyncSocketClientTransport(asio::ip::tcp::endpoint &endpoint);

    asio::io_context &getContext();

    void connect() override;
    int read(uint8_t *buffer, size_t len) override;
    bool write(uint8_t *buffer, size_t len) override;
    void subscribe(std::function<void(uint8_t *, size_t)> callback) override;

private:
    asio::awaitable<void> listener(std::function<void(uint8_t *, size_t)> callback);
};

} // namespace ThingSet