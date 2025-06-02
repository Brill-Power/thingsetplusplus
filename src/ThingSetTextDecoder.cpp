/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "thingset++/ThingSetTextDecoder.hpp"

namespace ThingSet {

ThingSetTextDecoder::ThingSetTextDecoder() : ThingSetTextDecoder(ThingSetTextDecoderOptions{})
{}

ThingSetTextDecoder::ThingSetTextDecoder(ThingSetTextDecoderOptions options) : _options(options)
{}

bool ThingSetTextDecoder::getIsForwardOnly() const
{
    return false;
}

bool ThingSetTextDecoder::decode(std::string *value)
{
    zcbor_string zstring;
    if (zcbor_tstr_decode(this->getState(), &zstring)) {
        *value = std::string((char *)zstring.value, zstring.len);
        return true;
    }
    return false;
}

bool ThingSetTextDecoder::decode(char *value)
{
    zcbor_string zstring;
    if (zcbor_tstr_decode(this->getState(), &zstring)) {
        strncpy(value, (char *)zstring.value, zstring.len);
        value[zstring.len] = '\0';
        return true;
    }
    return false;
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

    if (retVal > 1 || retVal < 0) {
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

bool ThingSetTextDecoder::decodeNull()
{
    return zcbor_nil_expect(this->getState(), NULL);
}

bool ThingSetTextDecoder::decodeListStart()
{
    return zcbor_list_start_decode(getState());
}

bool ThingSetTextDecoder::decodeListEnd()
{
    return zcbor_list_end_decode(getState());
}

bool ThingSetTextDecoder::decodeList(std::function<bool(size_t)> callback)
{
    if (!decodeListStart()) {
        return false;
    }

    size_t index = 0;
    while (getState()->elem_count != 0) {
        if (!callback(index++)) {
            return false;
        }
    }

    return decodeListEnd();
}

} // namespace ThingSet