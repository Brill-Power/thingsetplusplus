/*
 * Copyright (c) 2024 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */
#include "asio/ThingSetAsyncSocketClientTransport.hpp"

#include <asio/co_spawn.hpp>
#include <asio/detached.hpp>
#include <internal/thingset.h>

using asio::awaitable;
using asio::co_spawn;
using asio::detached;
using asio::use_awaitable;

namespace ThingSet {

ThingSetAsyncSocketClientTransport::ThingSetAsyncSocketClientTransport(asio::ip::tcp::endpoint &endpoint)
    : _ioContext(1), _requestResponseSocket(_ioContext), _subscribeSocket(_ioContext), _endpoint(endpoint)
{}

asio::io_context &ThingSetAsyncSocketClientTransport::getContext()
{
    return _ioContext;
}

void ThingSetAsyncSocketClientTransport::connect()
{
    asio::error_code error;
    _requestResponseSocket.connect(_endpoint, error);
    if (error) {
        throw std::system_error(error);
    }
    _subscribeSocket.open(asio::ip::udp::v4(), error);
    if (error) {
        throw std::system_error(error);
    }
}

int ThingSetAsyncSocketClientTransport::read(uint8_t *buffer, size_t len)
{
    auto data = asio::buffer(buffer, len);
    asio::error_code error;
    size_t read = _requestResponseSocket.read_some(data, error);
    if (error) {
        throw std::system_error(error);
    }
    return read;
}

bool ThingSetAsyncSocketClientTransport::write(uint8_t *buffer, size_t len)
{
    auto data = asio::buffer(buffer, len);
    asio::error_code error;
    size_t written = _requestResponseSocket.write_some(data, error);
    if (error) {
        throw std::system_error(error);
    }
    return written == len;
}

awaitable<void> ThingSetAsyncSocketClientTransport::listener(std::function<void(uint8_t *, size_t)> callback)
{
    for (;;) {
        auto buffer = asio::buffer(_buffer, 1024);
        size_t length = co_await _subscribeSocket.async_receive(buffer, use_awaitable);
        if (length > 1) {
            if (_buffer[0] == THINGSET_BIN_REPORT) {
                callback(&_buffer[3], length);
            }
        }
    }
}

void ThingSetAsyncSocketClientTransport::subscribe(std::function<void(uint8_t *, size_t)> callback)
{
    asio::ip::udp::endpoint localEndpoint(asio::ip::address_v4::any(), 9002);
    asio::error_code error;
    _subscribeSocket.bind(localEndpoint, error);
    if (error) {
        throw std::system_error(error);
    }
    co_spawn(_ioContext, listener(callback), detached);
}
} // namespace ThingSet