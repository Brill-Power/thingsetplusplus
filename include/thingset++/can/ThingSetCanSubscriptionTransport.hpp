/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "thingset++/ThingSetSubscriptionTransport.hpp"
#include "thingset++/can/ThingSetCanInterface.hpp"

namespace ThingSet::Can {

class ThingSetCanSubscriptionTransport : public ThingSetSubscriptionTransport<CanID>
{
protected:
    virtual ThingSetCanInterface &getInterface() = 0;
};

} // namespace ThingSet::Can