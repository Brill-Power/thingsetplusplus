/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#ifdef CONFIG_THINGSET_PLUS_PLUS_EEPROM

#include <zephyr/drivers/eeprom.h>
#include <zephyr/sys/crc.h>
#include "thingset++/StreamingThingSetBinaryDecoder.hpp"
#include "thingset++/internal/logging.hpp"
#include "ThingSetEeprom.hpp"

namespace ThingSet::Zephyr {

template <size_t Chunk = CONFIG_THINGSET_PLUS_PLUS_EEPROM_CHUNK_SIZE> class StreamingZephyrEepromThingSetBinaryDecoder :
    public StreamingThingSetBinaryDecoder<Chunk>,
    protected ThingSetEeprom
{
private:
    EepromHeader _header;

public:
    StreamingZephyrEepromThingSetBinaryDecoder(const device *device, off_t offset) :
        StreamingThingSetBinaryDecoder<Chunk>(1),
        ThingSetEeprom(device, offset)
    {
        int result = eeprom_read(device, offset, &_header, sizeof(_header));
        LOG_INFO("Streaming EEPROM load: ver %d, len %d, CRC 0x%.8x, offset 0x%0x", _header.version,
                _header.data_len, _header.crc, (int)_offset);
        _offset += sizeof(_header);
        _localOffset = _offset;
        if (result == 0) {
            // fill the buffer for the initial read
            read(0, Chunk * 2);
        }
    }

    bool verify()
    {
        if (_header.crc != _crc) {
            LOG_WARN("Streaming EEPROM load: CRC mismatch 0x%.8x vs expected 0x%.8x", _crc, _header.crc);
            return false;
        }

        return true;
    }

    bool isEmpty()
    {
        if (unlikely(
                (_header.version == 0xFFFF && _header.data_len == 0xFFFF && _header.crc == 0xFFFFFFFF)
                || (_header.version == 0U && _header.data_len == 0U && _header.crc == 0U)))
        {
            LOG_WARN("EEPROM empty, keeping default values for data objects");
            return true;
        }

        return false;
    }

    uint16_t getVersion()
    {
        return _header.version;
    }

protected:
    int read() override
    {
        return read(Chunk, Chunk);
    }

private:
    int read(size_t pos, size_t maxSize)
    {
        size_t remaining = _header.data_len - (_offset - _localOffset);
        size_t chunk = MIN(remaining, maxSize);
        eeprom_read(_device, _offset, &this->_buffer[pos], chunk);
        _offset += chunk;
        _crc = crc32_ieee_update(_crc, &this->_buffer[pos], chunk);
        return chunk;
    }
};

} // namespace ThingSet::Zephyr

#endif // CONFIG_THINGSET_PLUS_PLUS_EEPROM
