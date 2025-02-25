/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "CanID.hpp"
#include "thingset++/ThingSetClientTransport.hpp"

namespace ThingSet::Can {

class ThingSetCanClientTransport : public ThingSetClientTransport
{
private:
    uint8_t _targetNodeAddress;

public:
    ThingSetCanClientTransport(uint8_t targetNodeAddress);
    bool connect() override;
};

} // namespace ThingSet::Can