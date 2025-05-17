/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <asio/co_spawn.hpp>
#include <asio/detached.hpp>
#include <asio/ip/tcp.hpp>
#include <asio/ip/udp.hpp>
#include <asio/write.hpp>
#include <thingset++/ip/asio/ThingSetAsyncSocketServerTransport.hpp>

#define SOCKET_TRANSPORT_MAX_CONNECTIONS 10

using asio::awaitable;
using asio::co_spawn;
using asio::detached;
using asio::use_awaitable;
using asio::ip::tcp;
using asio::ip::udp;

namespace ThingSet::Ip::Async {

ThingSetAsyncSocketServerTransport::ThingSetAsyncSocketServerTransport(asio::io_context &ioContext) : _ioContext(ioContext), _publishSocket(ioContext), _signals(_ioContext, SIGINT, SIGTERM)
{
}

awaitable<void> ThingSetAsyncSocketServerTransport::handle(asio::ip::tcp::socket socket,
                                           std::function<int(asio::ip::tcp::endpoint &, uint8_t *, size_t, uint8_t *, size_t)> callback)
{
    char request[1024];
    char response[1024];
    for (;;) {
        std::size_t n = co_await socket.async_read_some(asio::buffer(request), use_awaitable);
        auto remoteEndpoint = socket.remote_endpoint();
        auto responseLength = callback(remoteEndpoint, (uint8_t *)request, n, (uint8_t *)response, sizeof(response));
        co_await async_write(socket, asio::buffer(response, responseLength), use_awaitable);
    }
}

ThingSetAsyncSocketServerTransport::~ThingSetAsyncSocketServerTransport()
{
    asio::error_code error;
    _publishSocket.shutdown(asio::socket_base::shutdown_type::shutdown_both, error);
    _publishSocket.close(error);
}

awaitable<void> ThingSetAsyncSocketServerTransport::listener(std::function<int(asio::ip::tcp::endpoint &, uint8_t *, size_t, uint8_t *, size_t)> callback)
{
    auto executor = co_await asio::this_coro::executor;
    tcp::acceptor acceptor(executor, { tcp::v4(), 9001 });
        for (;;) {
        tcp::socket socket = co_await acceptor.async_accept(use_awaitable);
        co_spawn(executor, handle(std::move(socket), callback), detached);
    }
}

bool ThingSetAsyncSocketServerTransport::listen(std::function<int(asio::ip::tcp::endpoint &, uint8_t *, size_t, uint8_t *, size_t)> callback)
{
    _signals.async_wait([&](auto, auto) { _ioContext.stop(); });

    asio::error_code error;
    _publishSocket.open(udp::v4(), error);
    if (error) {
        throw std::system_error(error);
    }
    _publishSocket.set_option(udp::socket::reuse_address(true));
    _publishSocket.set_option(asio::socket_base::broadcast(true));

    co_spawn(_ioContext, listener(callback), detached);

    return true;
}

bool ThingSetAsyncSocketServerTransport::publish(uint8_t *buffer, size_t len)
{
    if (_publishSocket.is_open()) {
        udp::endpoint endpoint(asio::ip::address_v4::broadcast(), 9002);
        size_t sent = _publishSocket.send_to(asio::buffer(buffer, len), endpoint);
        return sent == len;
    }
    return false;
}

} // namespace ThingSet::Ip::Async