/*
 * Copyright (c) 2024 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */
#include "ThingSetBinaryDecoder.hpp"

zcbor_state_t *ThingSet::ThingSetBinaryDecoder::getDecoder()
{
    return nullptr;
}

bool ThingSet::ThingSetBinaryDecoder::decode(float *value)
{
    return zcbor_float16_32_decode(this->getDecoder(), value);
}

bool ThingSet::ThingSetBinaryDecoder::decode(double *value)
{
    return zcbor_float64_decode(this->getDecoder(), value);
}

bool ThingSet::ThingSetBinaryDecoder::decode(bool *value)
{
    return zcbor_bool_decode(this->getDecoder(), value);
}

bool ThingSet::ThingSetBinaryDecoder::decode(uint8_t *value)
{
    uint32_t i;
    if (zcbor_uint32_decode(this->getDecoder(), &i) && i < UINT8_MAX) {
        *value = i;
        return true;
    }

    return false;
}

bool ThingSet::ThingSetBinaryDecoder::decode(uint16_t *value)
{
    uint32_t i;
    if (zcbor_uint32_decode(this->getDecoder(), &i) && i < UINT16_MAX) {
        *value = i;
        return true;
    }

    return false;
}

bool ThingSet::ThingSetBinaryDecoder::decode(uint32_t *value)
{
    return zcbor_uint32_decode(this->getDecoder(), value);
}

bool ThingSet::ThingSetBinaryDecoder::decode(uint64_t *value)
{
    return zcbor_uint64_decode(this->getDecoder(), value);
}

bool ThingSet::ThingSetBinaryDecoder::decode(int8_t *value)
{
    int32_t i;
    if (zcbor_int32_decode(this->getDecoder(), &i) && i < INT8_MAX) {
        *value = i;
        return true;
    }

    return false;
}

bool ThingSet::ThingSetBinaryDecoder::decode(int16_t *value)
{
    int32_t i;
    if (zcbor_int32_decode(this->getDecoder(), &i) && i < INT16_MAX) {
        *value = i;
        return true;
    }

    return false;
}

bool ThingSet::ThingSetBinaryDecoder::decode(int32_t *value)
{
    return zcbor_int32_decode(this->getDecoder(), value);
}

bool ThingSet::ThingSetBinaryDecoder::decode(int64_t *value)
{
    return zcbor_int64_decode(this->getDecoder(), value);
}
