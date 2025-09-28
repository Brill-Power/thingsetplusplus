/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#ifdef CONFIG_THINGSET_PLUS_PLUS_EEPROM

#include <zephyr/device.h>
#include "thingset++/zephyr/StreamingZephyrEepromThingSetBinaryEncoder.hpp"
#include "thingset++/zephyr/StreamingZephyrEepromThingSetBinaryDecoder.hpp"

namespace ThingSet {

class ThingSetPersistence
{
private:
    const device *_device;

public:
    ThingSetPersistence(const device *device);

    bool load();
    bool save();
};

}

#endif // CONFIG_THINGSET_PLUS_PLUS_EEPROM