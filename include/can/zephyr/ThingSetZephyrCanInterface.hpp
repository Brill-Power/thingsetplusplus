/*
 * Copyright (c) 2024 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */
#pragma once

#include "can/ThingSetCanInterface.hpp"
#include <functional>
#include <zephyr/device.h>

namespace ThingSet::Can::Zephyr {

/// @brief Encapsulates a ThingSet CAN interface.
class ThingSetZephyrCanInterface : public ThingSetCanInterface
{
private:
    const device *const _canDevice;

public:
    ThingSetZephyrCanInterface(const device *const canDevice);
    ~ThingSetZephyrCanInterface();

    using ThingSetCanInterface::bind;
    bool bind(uint8_t nodeAddress) override;

    bool listen(std::function<int(uint8_t *, size_t, uint8_t *, size_t)> callback) override;

    bool publish(CanID &id, uint8_t *buffer, size_t length) override;
};

} // namespace ThingSet::Can::Zephyr