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

protected:
    ThingSetCanInterface &getInterface() override;

public:
    ThingSetZephyrCanServerTransport(const device *const canDevice);
};

} // namespace ThingSet::Can::Zephyr