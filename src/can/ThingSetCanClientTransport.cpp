/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "thingset++/can/ThingSetCanClientTransport.hpp"

namespace ThingSet::Can {

ThingSetCanClientTransport::ThingSetCanClientTransport(uint8_t targetNodeAddress)
    : _targetNodeAddress(targetNodeAddress)
{}

bool ThingSetCanClientTransport::connect()
{
    return true;
}

} // namespace ThingSet::Can