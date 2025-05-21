/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "thingset++/ip/udp/ThingSetUdpServerTransport.hpp"
#include <asio/awaitable.hpp>
#include <asio/signal_set.hpp>
#include <asio/ip/tcp.hpp>
#include <asio/ip/udp.hpp>

namespace ThingSet::Ip::Async {

using ThingSet::Ip::Udp::ThingSetUdpServerTransport;

class ThingSetAsyncSocketServerTransport : public ThingSetUdpServerTransport<asio::ip::tcp::endpoint>
{
private:
    asio::io_context &_ioContext;
    asio::ip::udp::socket _publishSocket;
    asio::signal_set _signals;

public:
    ThingSetAsyncSocketServerTransport(asio::io_context &ioContext);
    ~ThingSetAsyncSocketServerTransport();

    bool listen(std::function<int(const asio::ip::tcp::endpoint &, uint8_t *, size_t, uint8_t *, size_t)> callback) override;

    bool publish(uint8_t *buffer, size_t len) override;

private:
    asio::awaitable<void> handle(asio::ip::tcp::socket socket,
                                 std::function<int(const asio::ip::tcp::endpoint &, uint8_t *, size_t, uint8_t *, size_t)> callback);
    asio::awaitable<void> listener(std::function<int(const asio::ip::tcp::endpoint &, uint8_t *, size_t, uint8_t *, size_t)> callback);
};

} // namespace ThingSet::Ip::Async