/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "thingset++/ThingSetTextDecoder.hpp"

namespace ThingSet {

bool ThingSetTextDecoder::getIsForwardOnly() const
{
    return false;
}

bool ThingSetTextDecoder::decode(std::string *value)
{
    *value = _inputBuffer;
    return true;
}

bool ThingSetTextDecoder::decode(char *value)
{
    *value = _inputBuffer[0];
    return true;
}

bool ThingSetTextDecoder::decode(float *value)
{
    *value = std::strtof(_inputBuffer, NULL);
    return true;
}

bool ThingSetTextDecoder::decode(double *value)
{
    *value = std::strtod(_inputBuffer, NULL);
    return true;
}

// todo simplify this?
bool ThingSetTextDecoder::decode(bool *value)
{
    uint8_t retVal = std::strtoul(_inputBuffer, NULL, 10);

    if (retVal > 1) {
        return false;
    }

    *value = retVal;

    return true;
}

bool ThingSetTextDecoder::decode(uint8_t *value)
{
    *value = std::strtoul(_inputBuffer, NULL, 10);
    return true;
}

bool ThingSetTextDecoder::decode(uint16_t *value)
{
    *value = std::strtoul(_inputBuffer, NULL, 10);
    return true;
}

bool ThingSetTextDecoder::decode(uint32_t *value)
{
    *value = std::strtoul(_inputBuffer, NULL, 10);
    return true;
}

bool ThingSetTextDecoder::decode(uint64_t *value)
{
    *value = std::strtoull(_inputBuffer, NULL, 10);
    return true;
}

bool ThingSetTextDecoder::decode(int8_t *value)
{
    *value = std::strtol(_inputBuffer, NULL, 10);
    return true;
}

bool ThingSetTextDecoder::decode(int16_t *value)
{
    *value = std::strtol(_inputBuffer, NULL, 10);
    return true;
}

bool ThingSetTextDecoder::decode(int32_t *value)
{
    *value = std::strtol(_inputBuffer, NULL, 10);
    return true;
}

bool ThingSetTextDecoder::decode(int64_t *value)
{
    *value = std::strtoll(_inputBuffer, NULL, 10);
    return true;
}

// todo check this
bool ThingSetTextDecoder::decodeNull()
{
    return true;
}

// todo implement this
bool ThingSetTextDecoder::decodeListStart()
{
    return true;
}

// todo implement this
bool ThingSetTextDecoder::decodeListEnd()
{
    return true;
}

// todo implement this
bool ThingSetTextDecoder::decodeList(std::function<bool(size_t)> callback)
{
    // if (!decodeListStart()) {
    //     return false;
    // }

    // size_t index = 0;
    // while (getState()->elem_count != 0) {
    //     if (!callback(index++)) {
    //         return false;
    //     }
    // }

    // return decodeListEnd();
    (void)callback;
    return true;
}

} // namespace ThingSet