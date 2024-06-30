/*
 * Copyright (c) 2024 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */
#pragma once

#include "ThingSetServerTransport.hpp"
#include <asio/awaitable.hpp>
#include <asio/ip/tcp.hpp>

namespace ThingSet {

class ThingSetAsyncSocketServerTransport : public ThingSetServerTransport
{
private:
    asio::io_context _ioContext;

public:
    ThingSetAsyncSocketServerTransport();

    bool listen(std::function<std::pair<uint8_t *, size_t>(uint8_t *, size_t)> callback) override;

private:
    asio::awaitable<void> handle(asio::ip::tcp::socket socket,
                                 std::function<std::pair<uint8_t *, size_t>(uint8_t *, size_t)> callback);
    asio::awaitable<void> listener(std::function<std::pair<uint8_t *, size_t>(uint8_t *, size_t)> callback);
};

} // namespace ThingSet