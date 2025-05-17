/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "CanID.hpp"
#include <functional>

namespace ThingSet::Can {

/// @brief Encapsulates a ThingSet CAN interface.
class ThingSetCanInterface
{
protected:
    uint8_t _nodeAddress;
    std::function<void(const uint8_t *, uint8_t)> _addressClaimCallback;

public:
    uint8_t getNodeAddress();

    bool bind();
    virtual bool bind(uint8_t nodeAddress) = 0;

    void setAddressClaimCallback(std::function<void(const uint8_t *, uint8_t)> callback);
};

} // namespace ThingSet::Can