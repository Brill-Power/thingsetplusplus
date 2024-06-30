/*
 * Copyright (c) 2024 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */
#include "ThingSetBinaryDecoder.hpp"

bool ThingSet::ThingSetBinaryDecoder::decode(std::string *value)
{
    zcbor_string zstring;
    if (zcbor_tstr_decode(this->getState(), &zstring)) {
        *value = std::string((char *)zstring.value, zstring.len);
        return true;
    }
    return false;
}

bool ThingSet::ThingSetBinaryDecoder::decode(char *value)
{
    zcbor_string zstring;
    if (zcbor_tstr_decode(this->getState(), &zstring)) {
        strncpy(value, (char *)zstring.value, zstring.len);
        value[zstring.len] = '\0';
        return true;
    }
    return false;
}

bool ThingSet::ThingSetBinaryDecoder::decode(float *value)
{
    return zcbor_float16_32_decode(this->getState(), value);
}

bool ThingSet::ThingSetBinaryDecoder::decode(double *value)
{
    return zcbor_float64_decode(this->getState(), value);
}

bool ThingSet::ThingSetBinaryDecoder::decode(bool *value)
{
    return zcbor_bool_decode(this->getState(), value);
}

bool ThingSet::ThingSetBinaryDecoder::decode(uint8_t *value)
{
    uint32_t i;
    if (zcbor_uint32_decode(this->getState(), &i) && i < UINT8_MAX) {
        *value = i;
        return true;
    }

    return false;
}

bool ThingSet::ThingSetBinaryDecoder::decode(uint16_t *value)
{
    uint32_t i;
    if (zcbor_uint32_decode(this->getState(), &i) && i < UINT16_MAX) {
        *value = i;
        return true;
    }

    return false;
}

bool ThingSet::ThingSetBinaryDecoder::decode(uint32_t *value)
{
    return zcbor_uint32_decode(this->getState(), value);
}

bool ThingSet::ThingSetBinaryDecoder::decode(uint64_t *value)
{
    return zcbor_uint64_decode(this->getState(), value);
}

bool ThingSet::ThingSetBinaryDecoder::decode(int8_t *value)
{
    int32_t i;
    if (zcbor_int32_decode(this->getState(), &i) && i < INT8_MAX) {
        *value = i;
        return true;
    }

    return false;
}

bool ThingSet::ThingSetBinaryDecoder::decode(int16_t *value)
{
    int32_t i;
    if (zcbor_int32_decode(this->getState(), &i) && i < INT16_MAX) {
        *value = i;
        return true;
    }

    return false;
}

bool ThingSet::ThingSetBinaryDecoder::decode(int32_t *value)
{
    return zcbor_int32_decode(this->getState(), value);
}

bool ThingSet::ThingSetBinaryDecoder::decode(int64_t *value)
{
    return zcbor_int64_decode(this->getState(), value);
}

bool ThingSet::ThingSetBinaryDecoder::decodeNull()
{
    return zcbor_nil_expect(this->getState(), NULL);
}

bool ThingSet::ThingSetBinaryDecoder::peekIsList()
{
    zcbor_major_type_t type = ZCBOR_MAJOR_TYPE(this->getState()->payload[0]);
    return type == ZCBOR_MAJOR_TYPE_LIST;
}
