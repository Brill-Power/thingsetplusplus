/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "ThingSetZephyrCanInterface.hpp"
#include "thingset++/can/ThingSetCanServerTransport.hpp"

namespace ThingSet::Can::Zephyr {

class ThingSetZephyrCanServerTransport : public Can::ThingSetCanServerTransport
{
private:
    _ThingSetZephyrCanInterface &_canInterface;

protected:
    ThingSetCanInterface &getInterface() override;

public:
    ThingSetZephyrCanServerTransport(_ThingSetZephyrCanInterface &canInterface);

    bool bind(uint8_t nodeAddress);
};

} // namespace ThingSet::Can::Zephyr