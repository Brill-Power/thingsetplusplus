/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "thingset++/ThingSetClientTransport.hpp"
#include "thingset++/can/ThingSetCanInterface.hpp"

namespace ThingSet::Can {

class ThingSetCanClientTransport : public ThingSetClientTransport
{
protected:
    const uint8_t _targetNodeAddress;

protected:
    ThingSetCanClientTransport(const uint8_t targetNodeAddress);
};

} // namespace ThingSet::Can