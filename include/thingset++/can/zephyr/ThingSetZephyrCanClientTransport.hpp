/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "thingset++/can/zephyr/ThingSetZephyrCanInterface.hpp"
#include "thingset++/can/zephyr/ThingSetZephyrCanRequestResponseContext.hpp"
#include "thingset++/can/ThingSetCanClientTransport.hpp"

namespace ThingSet::Can::Zephyr {

class ThingSetZephyrCanClientTransport : public ThingSetCanClientTransport
{
private:
    ThingSetZephyrCanRequestResponseContext _requestResponseContext;
    k_msgq _responseQueue;
    struct ResponseMessage
    {
        char buffer[CONFIG_THINGSET_PLUS_PLUS_CAN_CLIENT_RX_BUFFER_SIZE];
        size_t length;
    };
    std::array<char, sizeof(ResponseMessage)> _responseQueueBuffer;

public:
    ThingSetZephyrCanClientTransport(ThingSetZephyrCanClientTransport &&) = delete;
    ThingSetZephyrCanClientTransport(const ThingSetZephyrCanClientTransport &) = delete;
    template <size_t RxSize, size_t TxSize>
    ThingSetZephyrCanClientTransport(ThingSetZephyrCanInterface &zephyrCanInterface, uint8_t targetNodeAddress,
                                     std::array<uint8_t, RxSize> &rxStorage, std::array<uint8_t, TxSize> &txStorage)
        : ThingSetCanClientTransport(targetNodeAddress),
          _requestResponseContext(zephyrCanInterface, rxStorage, txStorage)
    {
        k_msgq_init(&_responseQueue, _responseQueueBuffer.data(), sizeof(ResponseMessage), 1);
    }

    bool connect() override;

    int read(uint8_t *buffer, size_t len) override;
    bool write(uint8_t *buffer, size_t len) override;
};

} // namespace ThingSet::Can::Zephyr