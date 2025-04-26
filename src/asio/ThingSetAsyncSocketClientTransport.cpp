/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "thingset++/asio/ThingSetAsyncSocketClientTransport.hpp"
#include "thingset++/ThingSetStatus.hpp"
#include <asio/co_spawn.hpp>
#include <asio/detached.hpp>

using asio::awaitable;
using asio::co_spawn;
using asio::detached;
using asio::use_awaitable;

namespace ThingSet::Async {

ThingSetAsyncSocketClientTransport::ThingSetAsyncSocketClientTransport(asio::ip::tcp::endpoint &endpoint)
    : _ioContext(1), _requestResponseSocket(_ioContext), _subscribeSocket(_ioContext), _endpoint(endpoint)
{}

asio::io_context &ThingSetAsyncSocketClientTransport::getContext()
{
    return _ioContext;
}

bool ThingSetAsyncSocketClientTransport::connect()
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
    return true;
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
        if (length > 4) {
            if (_buffer[0] != ThingSetRequestType::report) {
                continue;
            }
            // get actual length from message body to check we have received everything
            size_t actualLength = _buffer[1] | (_buffer[2] << 8);
            if (actualLength == length - 3) {
                callback(&_buffer[3], actualLength);
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
} // namespace ThingSet::Async