/*
 * Copyright (c) 2024 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */
#include "thingset++/can/ThingSetCanInterface.hpp"
#include "thingset++/Eui.hpp"
#include "thingset++/can/CanID.hpp"
#include <chrono>
#include <random>
#include <string.h>

namespace ThingSet::Can {

uint8_t ThingSetCanInterface::getNodeAddress()
{
    return _nodeAddress;
}

bool ThingSetCanInterface::bind()
{
    return bind(CanID::minAddress);
}

} // namespace ThingSet::Can