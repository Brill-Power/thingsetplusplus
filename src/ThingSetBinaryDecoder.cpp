/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "thingset++/ThingSetBinaryDecoder.hpp"

#ifndef ZCBOR_MAJOR_TYPE
#define ZCBOR_MAJOR_TYPE(header_byte) ((zcbor_major_type_t)(((header_byte) >> 5) & 0x7))
#endif

namespace ThingSet {

ThingSetBinaryDecoder::ThingSetBinaryDecoder() : ThingSetBinaryDecoder(ThingSetBinaryDecoderOptions{})
{}

ThingSetBinaryDecoder::ThingSetBinaryDecoder(ThingSetBinaryDecoderOptions options) : _options(options)
{}

bool ThingSetBinaryDecoder::getIsForwardOnly() const
{
    return false;
}

bool ThingSetBinaryDecoder::decode(std::string *value)
{
    zcbor_string zstring;
    if (zcbor_tstr_decode(this->getState(), &zstring)) {
        *value = std::string((char *)zstring.value, zstring.len);
        return true;
    }
    return false;
}

bool ThingSetBinaryDecoder::decode(char *value)
{
    zcbor_string zstring;
    if (zcbor_tstr_decode(this->getState(), &zstring)) {
        strncpy(value, (char *)zstring.value, zstring.len);
        value[zstring.len] = '\0';
        return true;
    }
    return false;
}

bool ThingSetBinaryDecoder::decode(float *value)
{
    return zcbor_float16_32_decode(this->getState(), value);
}

bool ThingSetBinaryDecoder::decode(double *value)
{
    return zcbor_float64_decode(this->getState(), value);
}

bool ThingSetBinaryDecoder::decode(bool *value)
{
    return zcbor_bool_decode(this->getState(), value);
}

bool ThingSetBinaryDecoder::decode(uint8_t *value)
{
    uint32_t i;
    if (zcbor_uint32_decode(this->getState(), &i) && i < UINT8_MAX) {
        *value = i;
        return true;
    }

    return false;
}

bool ThingSetBinaryDecoder::decode(uint16_t *value)
{
    uint32_t i;
    if (zcbor_uint32_decode(this->getState(), &i) && i < UINT16_MAX) {
        *value = i;
        return true;
    }

    return false;
}

bool ThingSetBinaryDecoder::decode(uint32_t *value)
{
    return zcbor_uint32_decode(this->getState(), value);
}

bool ThingSetBinaryDecoder::decode(uint64_t *value)
{
    return zcbor_uint64_decode(this->getState(), value);
}

bool ThingSetBinaryDecoder::decode(int8_t *value)
{
    int32_t i;
    if (zcbor_int32_decode(this->getState(), &i) && i < INT8_MAX) {
        *value = i;
        return true;
    }

    return false;
}

bool ThingSetBinaryDecoder::decode(int16_t *value)
{
    int32_t i;
    if (zcbor_int32_decode(this->getState(), &i) && i < INT16_MAX) {
        *value = i;
        return true;
    }

    return false;
}

bool ThingSetBinaryDecoder::decode(int32_t *value)
{
    return zcbor_int32_decode(this->getState(), value);
}

bool ThingSetBinaryDecoder::decode(int64_t *value)
{
    return zcbor_int64_decode(this->getState(), value);
}

bool ThingSetBinaryDecoder::decodeNull()
{
    return zcbor_nil_expect(this->getState(), NULL);
}

bool ThingSetBinaryDecoder::decodeListStart()
{
    return zcbor_list_start_decode(getState());
}

bool ThingSetBinaryDecoder::decodeListEnd()
{
    return zcbor_list_end_decode(getState());
}

bool ThingSetBinaryDecoder::decodeList(std::function<bool(size_t)> callback)
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

zcbor_major_type_t ThingSetBinaryDecoder::peekType()
{
    zcbor_major_type_t type = ZCBOR_MAJOR_TYPE(this->getState()->payload[0]);
    return type;
}

bool ThingSetBinaryDecoder::skip()
{
    return zcbor_any_skip(this->getState(), NULL);
}

bool ThingSetBinaryDecoder::skipUntil(zcbor_major_type_t sought)
{
    zcbor_major_type_t type;
    while ((type = this->peekType()) != sought) {
        this->skip();
    };
    return sought == type;
}

} // namespace ThingSet