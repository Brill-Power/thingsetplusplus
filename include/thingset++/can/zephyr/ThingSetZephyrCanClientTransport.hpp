/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "ThingSetZephyrCanInterface.hpp"
#include "thingset++/can/ThingSetCanClientTransport.hpp"

namespace ThingSet::Can::Zephyr {

class ThingSetZephyrCanClientTransport : public ThingSetCanClientTransport
{
private:
    ThingSetZephyrCanInterface &_canInterface;

public:
    ThingSetZephyrCanClientTransport(ThingSetZephyrCanInterface &canInterface, uint8_t targetNodeAddress);

    int read(uint8_t *buffer, size_t len) override;
    bool write(uint8_t *buffer, size_t len) override;
};

} // namespace ThingSet::Can::Zephyr