/*
 * Copyright (c) 2024 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */

#include "ThingSetAsyncSocketServerTransport.hpp"
#include <asio/co_spawn.hpp>
#include <asio/detached.hpp>
#include <asio/ip/tcp.hpp>
#include <asio/signal_set.hpp>
#include <asio/write.hpp>

#define SOCKET_TRANSPORT_MAX_CONNECTIONS 10

using asio::awaitable;
using asio::co_spawn;
using asio::detached;
using asio::use_awaitable;
using asio::ip::tcp;

ThingSet::ThingSetAsyncSocketServerTransport::ThingSetAsyncSocketServerTransport() : _ioContext(1)
{}

asio::awaitable<void> ThingSet::ThingSetAsyncSocketServerTransport::handle(
    asio::ip::tcp::socket socket, std::function<std::pair<uint8_t *, size_t>(uint8_t *, size_t)> callback)
{
    char data[1024];
    for (;;) {
        std::size_t n = co_await socket.async_read_some(asio::buffer(data), use_awaitable);
        auto [response, responseLength] = callback((uint8_t *)data, n);
        co_await async_write(socket, asio::buffer(response, responseLength), use_awaitable);
    }
}

awaitable<void> ThingSet::ThingSetAsyncSocketServerTransport::listener(
    std::function<std::pair<uint8_t *, size_t>(uint8_t *, size_t)> callback)
{
    auto executor = co_await asio::this_coro::executor;
    tcp::acceptor acceptor(executor, { tcp::v4(), 9001 });
    for (;;) {
        tcp::socket socket = co_await acceptor.async_accept(use_awaitable);
        co_spawn(executor, handle(std::move(socket), callback), detached);
    }
}

bool ThingSet::ThingSetAsyncSocketServerTransport::listen(
    std::function<std::pair<uint8_t *, size_t>(uint8_t *, size_t)> callback)
{
    asio::signal_set signals(_ioContext, SIGINT, SIGTERM);
    signals.async_wait([&](auto, auto) { _ioContext.stop(); });

    co_spawn(_ioContext, listener(callback), detached);

    _ioContext.run();
    return true;
}
