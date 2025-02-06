/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
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

void ThingSetCanInterface::setAddressClaimCallback(std::function<void(const uint8_t *, uint8_t)> callback)
{
    _addressClaimCallback = callback;
}

} // namespace ThingSet::Can