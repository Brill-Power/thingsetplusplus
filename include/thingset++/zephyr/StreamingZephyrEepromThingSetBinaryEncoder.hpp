/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#ifdef CONFIG_THINGSET_PLUS_PLUS_EEPROM

#include <zephyr/drivers/eeprom.h>
#include <zephyr/sys/crc.h>
#include "thingset++/StreamingThingSetBinaryEncoder.hpp"
#include "thingset++/internal/logging.hpp"
#include "ThingSetEeprom.hpp"

namespace ThingSet::Zephyr {

/// @brief Encoder for persisting properties to EEPROM.
/// @tparam Chunk Size of chunked writes to EEPROM.
template <size_t Chunk = CONFIG_THINGSET_PLUS_PLUS_EEPROM_CHUNK_SIZE> class StreamingZephyrEepromThingSetBinaryEncoder :
    public virtual StreamingThingSetBinaryEncoder<Chunk>,
    protected ThingSetEeprom
{
private:
    size_t _initialOffset;

public:
    StreamingZephyrEepromThingSetBinaryEncoder(const device *device, off_t offset) :
        StreamingThingSetBinaryEncoder(),
        ThingSetEeprom(device, offset + sizeof(EepromHeader)), _initialOffset(offset)
    {
        zcbor_new_encode_state(_state, BINARY_ENCODER_DEFAULT_MAX_DEPTH, &_buffer[0],
                               _buffer.size(), 1);
    }

    bool write(size_t length, bool flushing) override
    {
        uint8_t chunk[Chunk];
        int err = 0;
        for (int i = 0; i < CONFIG_THINGSET_PLUS_PLUS_EEPROM_ACCESS_ATTEMPTS; i++) {
            int err = eeprom_write(_device, _offset, &_buffer[0], length);
            if (err) {
                LOG_WARN("EEPROM write error %d at offset 0x%lx", err, _offset);
                continue;
            }

            err = eeprom_read(_device, _offset, &chunk[0], length);
            if (err) {
                LOG_WARN("EEPROM read error %d at offset 0x%lx", err, _offset);
                continue;
            }

            err = memcmp(&_buffer[0], &chunk[0], length);
            if (err) {
                LOG_WARN("EEPROM data mismatch at offset 0x%lx", _offset);
                continue;
            }
            else {
                break;
            }
        }
        if (err) {
            /* writing has failed epically; bail */
            LOG_ERROR("Out of retries, abandoning EEPROM write");
            return false;
        }
        _crc = crc32_ieee_update(_crc, &_buffer[0], length);
        _offset += length;
        if (flushing) {
            // go back and update header
            EepromHeader header = { .version = CONFIG_THINGSET_PLUS_PLUS_EEPROM_DATA_VERSION,
                                    .data_len = (uint16_t)(_offset - _initialOffset - sizeof(header)),
                                    .crc = _crc };
            eeprom_write(_device, _initialOffset, &header, sizeof(header));
            LOG_INFO("Streaming EEPROM save: ver %d, len %d, CRC 0x%.8x, offset 0x%0x", header.version,
                     header.data_len, header.crc, (int)_offset);
        }
        return true;
    }
};

} // namespace ThingSet::Zephyr

#endif // CONFIG_THINGSET_PLUS_PLUS_EEPROM
