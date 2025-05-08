/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "thingset++/ThingSetSubscriptionTransport.hpp"
#include "thingset++/can/CanID.hpp"

namespace ThingSet::Can {

class ThingSetCanSubscriptionTransport : public ThingSetSubscriptionTransport<CanID>
{
public:
    void subscribe(std::function<void(CanID &, uint8_t *, size_t)> callback) override;
};

} // namespace ThingSet::Can