/*
 * Copyright (c) 2024 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */
#pragma once

#include "can/ThingSetCanServerTransport.hpp"
#include "can/zephyr/ThingSetZephyrCanInterface.hpp"

namespace ThingSet::Can::Zephyr {

class ThingSetZephyrCanServerTransport : public Can::ThingSetCanServerTransport
{
private:
    ThingSetZephyrCanInterface _canInterface;

public:
    ThingSetZephyrCanServerTransport(const device *const canDevice);

    bool listen(std::function<int(uint8_t *, size_t, uint8_t *, size_t)> callback) override;
    bool publish(Can::CanID &id, uint8_t *buffer, size_t length) override;
};

} // namespace ThingSet::Can::Zephyr