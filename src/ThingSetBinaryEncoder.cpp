/*
 * Copyright (c) 2024 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */
#include "ThingSetBinaryEncoder.hpp"

zcbor_state_t *ThingSet::ThingSetBinaryEncoder::getEncoder()
{
    return nullptr;
}

size_t ThingSet::ThingSetBinaryEncoder::getEncodedLength()
{
    return 0;
}

bool ThingSet::ThingSetBinaryEncoder::encode(char *&value)
{
    return zcbor_tstr_put_term(this->getEncoder(), value, BINARY_ENCODER_MAX_NULL_TERMINATED_STRING_LENGTH);
}

bool ThingSet::ThingSetBinaryEncoder::encode(float &value)
{
    return zcbor_float32_put(this->getEncoder(), value);
}

bool ThingSet::ThingSetBinaryEncoder::encode(double &value)
{
    return zcbor_float64_put(this->getEncoder(), value);
}

bool ThingSet::ThingSetBinaryEncoder::encode(bool &value)
{
    return zcbor_bool_put(this->getEncoder(), value);
}

bool ThingSet::ThingSetBinaryEncoder::encode(uint8_t &value)
{
    return zcbor_uint32_put(this->getEncoder(), value);
}

bool ThingSet::ThingSetBinaryEncoder::encode(uint16_t &value)
{
    return zcbor_uint32_put(this->getEncoder(), value);
}

bool ThingSet::ThingSetBinaryEncoder::encode(uint32_t &value)
{
    return zcbor_uint32_put(this->getEncoder(), value);
}

bool ThingSet::ThingSetBinaryEncoder::encode(uint64_t &value)
{
    return zcbor_uint64_put(this->getEncoder(), value);
}

bool ThingSet::ThingSetBinaryEncoder::encode(int8_t &value)
{
    return zcbor_int32_put(this->getEncoder(), value);
}

bool ThingSet::ThingSetBinaryEncoder::encode(int16_t &value)
{
    return zcbor_int32_put(this->getEncoder(), value);
}

bool ThingSet::ThingSetBinaryEncoder::encode(int32_t &value)
{
    return zcbor_int32_put(this->getEncoder(), value);
}

bool ThingSet::ThingSetBinaryEncoder::encode(int64_t &value)
{
    return zcbor_int64_put(this->getEncoder(), value);
}
