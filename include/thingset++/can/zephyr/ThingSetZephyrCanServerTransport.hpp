/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "thingset++/can/zephyr/ThingSetZephyrCanInterface.hpp"
#include "thingset++/can/ThingSetCanServerTransport.hpp"

namespace ThingSet::Can::Zephyr {

class ThingSetZephyrCanServerTransport : public Can::ThingSetCanServerTransport
{
private:
    _ThingSetZephyrCanInterface &_canInterface;

protected:
    ThingSetCanInterface &getInterface() override;

    bool doPublish(Can::CanID &id, uint8_t *buffer, size_t length) override;

public:
    ThingSetZephyrCanServerTransport(_ThingSetZephyrCanInterface &canInterface);
};

} // namespace ThingSet::Can::Zephyr