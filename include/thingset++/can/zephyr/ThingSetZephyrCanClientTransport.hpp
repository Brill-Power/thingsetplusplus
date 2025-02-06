/*
 * Copyright (c) 2024 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */
#pragma once

#include "thingset++/can/ThingSetCanClientTransport.hpp"
#include "ThingSetZephyrCanInterface.hpp"

namespace ThingSet::Can::Zephyr {

class ThingSetZephyrCanClientTransport : public ThingSetCanClientTransport {
private:
    ThingSetZephyrCanInterface &_canInterface;

public:
    ThingSetZephyrCanClientTransport(ThingSetZephyrCanInterface &canInterface, uint8_t targetNodeAddress);

    int read(uint8_t *buffer, size_t len) override;
    bool write(uint8_t *buffer, size_t len) override;
};

} // ThingSet::Can::Zephyr