/*
 * Copyright (c) 2024 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */
#pragma once

#include "thingset++/ThingSetClientTransport.hpp"
#include "CanID.hpp"

namespace ThingSet::Can {

class ThingSetCanClientTransport : public ThingSetClientTransport {
private:
    uint8_t _targetNodeAddress;

public:
    ThingSetCanClientTransport(uint8_t targetNodeAddress);
    bool connect() override;
};

} // namespace ThingSet::Can