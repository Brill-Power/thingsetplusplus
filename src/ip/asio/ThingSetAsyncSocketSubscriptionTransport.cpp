/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "thingset++/ip/asio/ThingSetAsyncSocketSubscriptionTransport.hpp"
#include "thingset++/ThingSetStatus.hpp"
#include <asio/co_spawn.hpp>
#include <asio/detached.hpp>

using asio::awaitable;
using asio::co_spawn;
using asio::detached;
using asio::use_awaitable;

namespace ThingSet::Ip::Async {

ThingSetAsyncSocketSubscriptionTransport::ThingSetAsyncSocketSubscriptionTransport(asio::io_context &ioContext) : _ioContext(ioContext), _subscribeSocket(ioContext)
{}

ThingSetAsyncSocketSubscriptionTransport::~ThingSetAsyncSocketSubscriptionTransport()
{
    asio::error_code error;
    _subscribeSocket.shutdown(asio::socket_base::shutdown_type::shutdown_both, error);
    _subscribeSocket.close(error);
}

awaitable<void> ThingSetAsyncSocketSubscriptionTransport::listener(std::function<void(const asio::ip::udp::endpoint &, ThingSetBinaryDecoder &)> callback)
{
    for (;;) {
        auto buffer = asio::buffer(_buffer, 1024);
        asio::ip::udp::endpoint sender;
        size_t length = co_await _subscribeSocket.async_receive_from(buffer, sender, use_awaitable);
        if (length > 4) {
            if (_buffer[0] != ThingSetBinaryRequestType::report) {
                continue;
            }
            // get actual length from message body to check we have received everything
            size_t actualLength = _buffer[1] | (_buffer[2] << 8);
            if (actualLength == length - 3) {
                DefaultFixedDepthThingSetBinaryDecoder decoder(&_buffer[3], actualLength, 2);
                callback(sender, decoder);
            }
        }
    }
}

bool ThingSetAsyncSocketSubscriptionTransport::subscribe(std::function<void(const asio::ip::udp::endpoint &, ThingSetBinaryDecoder &)> callback)
{
    asio::error_code error;
    _subscribeSocket.open(asio::ip::udp::v4(), error);
    _subscribeSocket.set_option(asio::ip::udp::socket::reuse_address(true));
    if (error) {
        throw std::system_error(error);
    }

    asio::ip::udp::endpoint localEndpoint(asio::ip::address_v4::any(), 9002);
    _subscribeSocket.bind(localEndpoint, error);
    if (error) {
        throw std::system_error(error);
    }
    co_spawn(_ioContext, listener(callback), detached);
    return true;
}

} // namespace ThingSet::Ip::Async