/*
 * Copyright (c) 2024 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */
#pragma once

#include "thingset++/can/ThingSetCanServerTransport.hpp"
#include "ThingSetZephyrCanInterface.hpp"

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