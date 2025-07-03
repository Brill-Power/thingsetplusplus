/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "thingset++/can/zephyr/ThingSetZephyrCanInterface.hpp"
#include "thingset++/can/ThingSetCanClientTransport.hpp"

namespace ThingSet::Can::Zephyr {

class ThingSetZephyrCanClientTransport : public ThingSetCanClientTransport
{
private:
    ThingSetZephyrCanInterface &_canInterface;
    k_msgq _responseQueue;
    struct ResponseMessage
    {
        char buffer[1024];
        size_t length;
    };
    std::array<char, sizeof(ResponseMessage)> _responseQueueBuffer;

public:
    ThingSetZephyrCanClientTransport(ThingSetZephyrCanClientTransport &&) = delete;
    ThingSetZephyrCanClientTransport(const ThingSetZephyrCanClientTransport &) = delete;
    ThingSetZephyrCanClientTransport(ThingSetZephyrCanInterface &canInterface, uint8_t targetNodeAddress);

    bool connect() override;

    int read(uint8_t *buffer, size_t len) override;
    bool write(uint8_t *buffer, size_t len) override;
};

} // namespace ThingSet::Can::Zephyr