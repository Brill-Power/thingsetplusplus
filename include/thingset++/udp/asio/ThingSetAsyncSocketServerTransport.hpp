/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "thingset++/ThingSetServerTransport.hpp"
#include <asio/awaitable.hpp>
#include <asio/ip/tcp.hpp>
#include <asio/ip/udp.hpp>

namespace ThingSet::Udp::Async {

class ThingSetAsyncSocketServerTransport : public ThingSetServerTransport
{
private:
    asio::io_context _ioContext;
    asio::ip::udp::socket _publishSocket;

public:
    ThingSetAsyncSocketServerTransport();

    asio::io_context &getContext();

    bool listen(std::function<int(uint8_t *, size_t, uint8_t *, size_t)> callback) override;

    bool publish(uint8_t *buffer, size_t len) override;

private:
    asio::awaitable<void> handle(asio::ip::tcp::socket socket,
                                 std::function<int(uint8_t *, size_t, uint8_t *, size_t)> callback);
    asio::awaitable<void> listener(std::function<int(uint8_t *, size_t, uint8_t *, size_t)> callback);
};

} // namespace ThingSet::Async