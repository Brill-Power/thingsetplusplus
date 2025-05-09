/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "thingset++/ThingSetClientTransport.hpp"
#include <asio/ip/tcp.hpp>
#include <cstdint>
#include <cstdio>

namespace ThingSet::Udp::Async {

class ThingSetAsyncSocketClientTransport : public ThingSetClientTransport
{
private:
    asio::ip::tcp::socket _requestResponseSocket;
    asio::ip::tcp::endpoint &_endpoint;

public:
    ThingSetAsyncSocketClientTransport(asio::io_context &ioContext, asio::ip::tcp::endpoint &endpoint);
    ~ThingSetAsyncSocketClientTransport();

    bool connect() override;
    int read(uint8_t *buffer, size_t len) override;
    bool write(uint8_t *buffer, size_t len) override;
};

} // namespace ThingSet::Async