/*
 * Copyright (c) 2024 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
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

public:
    uint8_t getNodeAddress();

    bool bind();
    virtual bool bind(uint8_t nodeAddress) = 0;

    virtual bool listen(std::function<int(uint8_t *, size_t, uint8_t *, size_t)> callback) = 0;

    virtual bool publish(CanID &id, uint8_t *buffer, size_t length) = 0;
};

} // namespace ThingSet::Can