/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "thingset++/can/zephyr/ThingSetZephyrCanServerTransport.hpp"

namespace ThingSet::Can::Zephyr {

ThingSetZephyrCanServerTransport::ThingSetZephyrCanServerTransport(_ThingSetZephyrCanInterface &canInterface)
    : ThingSetCanServerTransport(), _canInterface(canInterface)
{}

ThingSetCanInterface &ThingSetZephyrCanServerTransport::getInterface()
{
    return _canInterface;
}

bool ThingSetZephyrCanServerTransport::bind(uint8_t nodeAddress)
{
    return _canInterface.bind(nodeAddress);
}

} // namespace ThingSet::Can::Zephyr