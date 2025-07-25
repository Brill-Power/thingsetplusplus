/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "thingset++/ip/asio/ThingSetAsyncSocketSubscriptionTransport.hpp"
#include "thingset++/ip/StreamingUdpThingSetBinaryDecoder.hpp"
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
    std::map<asio::ip::udp::endpoint, StreamingUdpThingSetBinaryDecoder>  decodersBySender;
    for (;;) {
        Frame frame;
        auto buffer = asio::buffer(frame.buffer, THINGSET_STREAMING_MSG_SIZE);
        asio::ip::udp::endpoint sender;
        frame.length = co_await _subscribeSocket.async_receive_from(buffer, sender, use_awaitable);
        SubscriptionListener::handle(frame, sender, sender, decodersBySender, callback);
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