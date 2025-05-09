/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "thingset++/udp/asio/ThingSetAsyncSocketClientTransport.hpp"
#include "thingset++/ThingSetStatus.hpp"

namespace ThingSet::Udp::Async {

ThingSetAsyncSocketClientTransport::ThingSetAsyncSocketClientTransport(asio::io_context &ioContext, asio::ip::tcp::endpoint &endpoint)
    : _requestResponseSocket(ioContext), _endpoint(endpoint)
{}

ThingSetAsyncSocketClientTransport::~ThingSetAsyncSocketClientTransport()
{
    asio::error_code error;
    _requestResponseSocket.shutdown(asio::socket_base::shutdown_type::shutdown_both, error);
    _requestResponseSocket.close(error);
}

bool ThingSetAsyncSocketClientTransport::connect()
{
    asio::error_code error;
    _requestResponseSocket.connect(_endpoint, error);
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

} // namespace ThingSet::Async