/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "thingset++/ip/ThingSetIpServerTransport.hpp"
#include <asio/awaitable.hpp>
#include <asio/signal_set.hpp>
#include <asio/ip/tcp.hpp>
#include <asio/ip/udp.hpp>

namespace ThingSet::Ip::Async {

class ThingSetAsyncSocketServerTransport : public ThingSetIpServerTransport<asio::ip::tcp::endpoint>
{
private:
    asio::io_context &_ioContext;
    asio::ip::udp::socket _publishSocket;
    asio::ip::address_v4 _bindAddress;
    asio::ip::address_v4 _broadcastAddress;
    asio::signal_set _signals;

public:
    ThingSetAsyncSocketServerTransport(asio::io_context &ioContext);
    ThingSetAsyncSocketServerTransport(asio::io_context &ioContext, const asio::ip::address_v4 bindAddress, const asio::ip::address_v4 broadcastAddress);
    ThingSetAsyncSocketServerTransport(asio::io_context &ioContext, const std::string &bindInterface);
    ~ThingSetAsyncSocketServerTransport();

    bool listen(std::function<int(const asio::ip::tcp::endpoint &, uint8_t *, size_t, uint8_t *, size_t)> callback) override;

    bool publish(uint8_t *buffer, size_t len) override;

private:
    asio::awaitable<void> handle(asio::ip::tcp::socket socket,
                                 std::function<int(const asio::ip::tcp::endpoint &, uint8_t *, size_t, uint8_t *, size_t)> callback);
    asio::awaitable<void> listener(std::function<int(const asio::ip::tcp::endpoint &, uint8_t *, size_t, uint8_t *, size_t)> callback);
};

} // namespace ThingSet::Ip::Async