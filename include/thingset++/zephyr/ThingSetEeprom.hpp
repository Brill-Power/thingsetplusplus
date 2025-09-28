/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#ifdef CONFIG_THINGSET_PLUS_PLUS_EEPROM

#include <cstdint>
#include <sys/types.h>
#include <zephyr/device.h>

namespace ThingSet::Zephyr {

/// @brief EEPROM header definition, etc.
class ThingSetEeprom
{
protected:
    struct EepromHeader
    {
        uint16_t version;
        uint16_t data_len;
        uint32_t crc;
    } __packed;

    const device *_device;
    off_t _offset;
    off_t _localOffset;
    uint32_t _crc;

    ThingSetEeprom(const device *device, off_t offset);
};

} // namespace ThingSet

#endif // CONFIG_THINGSET_PLUS_PLUS_EEPROM