/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "thingset++/udp/asio/ThingSetAsyncSocketSubscriptionTransport.hpp"
#include "thingset++/ThingSetStatus.hpp"
#include <asio/co_spawn.hpp>
#include <asio/detached.hpp>

using asio::awaitable;
using asio::co_spawn;
using asio::detached;
using asio::use_awaitable;

namespace ThingSet::Udp::Async {

ThingSetAsyncSocketSubscriptionTransport::ThingSetAsyncSocketSubscriptionTransport(asio::io_context &ioContext) : _ioContext(ioContext), _subscribeSocket(ioContext)
{}

bool ThingSetAsyncSocketSubscriptionTransport::connect()
{
    asio::error_code error;
    _subscribeSocket.open(asio::ip::udp::v4(), error);
    if (error) {
        throw std::system_error(error);
    }
    return true;
}

awaitable<void> ThingSetAsyncSocketSubscriptionTransport::listener(std::function<void(asio::ip::udp::endpoint &, uint8_t *, size_t)> callback)
{
    for (;;) {
        auto buffer = asio::buffer(_buffer, 1024);
        asio::ip::udp::endpoint sender;
        size_t length = co_await _subscribeSocket.async_receive_from(buffer, sender, use_awaitable);
        if (length > 4) {
            if (_buffer[0] != ThingSetRequestType::report) {
                continue;
            }
            // get actual length from message body to check we have received everything
            size_t actualLength = _buffer[1] | (_buffer[2] << 8);
            if (actualLength == length - 3) {
                callback(sender, &_buffer[3], actualLength);
            }
        }
    }
}

void ThingSetAsyncSocketSubscriptionTransport::subscribe(std::function<void(asio::ip::udp::endpoint &, uint8_t *, size_t)> callback)
{
    asio::ip::udp::endpoint localEndpoint(asio::ip::address_v4::any(), 9002);
    asio::error_code error;
    _subscribeSocket.bind(localEndpoint, error);
    if (error) {
        throw std::system_error(error);
    }
    co_spawn(_ioContext, listener(callback), detached);
}

}