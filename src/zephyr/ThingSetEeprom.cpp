/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "thingset++/zephyr/ThingSetEeprom.hpp"

namespace ThingSet::Zephyr {

ThingSetEeprom::ThingSetEeprom(const device *device, off_t offset)
    : _device(device), _offset(offset), _crc(0)
{}

}