/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "thingset++/can/zephyr/ThingSetZephyrCanClientTransport.hpp"

namespace ThingSet::Can::Zephyr {
ThingSetZephyrCanClientTransport::ThingSetZephyrCanClientTransport(ThingSetZephyrCanInterface &canInterface,
                                                                   uint8_t targetNodeAddress)
    : _canInterface(canInterface), ThingSetCanClientTransport(targetNodeAddress)
{}

int ThingSetZephyrCanClientTransport::read(uint8_t *buffer, size_t len)
{
    return 0;
}

bool ThingSetZephyrCanClientTransport::write(uint8_t *buffer, size_t len)
{
    return false;
}

} // namespace ThingSet::Can::Zephyr
