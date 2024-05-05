/*
 * Copyright (c) 2024 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */
#include "ThingSetBinaryEncoder.hpp"

zcbor_state_t *ThingSetBinaryEncoder::getEncoder()
{
    return nullptr;
}

size_t ThingSetBinaryEncoder::getEncodedLength()
{
    return 0;
}

template <> bool ThingSetBinaryEncoder::encode(char *&value)
{
    return zcbor_tstr_put_term(getEncoder(), value);
}

template <> bool ThingSetBinaryEncoder::encode(float &value)
{
    return zcbor_float32_put(getEncoder(), value);
}

template <> bool ThingSetBinaryEncoder::encode(double &value)
{
    return zcbor_float64_put(getEncoder(), value);
}

template <> bool ThingSetBinaryEncoder::encode(bool &value)
{
    return zcbor_bool_put(getEncoder(), value);
}

template <> bool ThingSetBinaryEncoder::encode(uint8_t &value)
{
    return zcbor_uint32_put(getEncoder(), value);
}

template <> bool ThingSetBinaryEncoder::encode(uint16_t &value)
{
    return zcbor_uint32_put(getEncoder(), value);
}

template <> bool ThingSetBinaryEncoder::encode(uint32_t &value)
{
    return zcbor_uint32_put(getEncoder(), value);
}

template <> bool ThingSetBinaryEncoder::encode(uint64_t &value)
{
    return zcbor_uint64_put(getEncoder(), value);
}

template <> bool ThingSetBinaryEncoder::encode(int8_t &value)
{
    return zcbor_int32_put(getEncoder(), value);
}

template <> bool ThingSetBinaryEncoder::encode(int16_t &value)
{
    return zcbor_int32_put(getEncoder(), value);
}

template <> bool ThingSetBinaryEncoder::encode(int32_t &value)
{
    return zcbor_int32_put(getEncoder(), value);
}

template <> bool ThingSetBinaryEncoder::encode(int64_t &value)
{
    return zcbor_int64_put(getEncoder(), value);
}
