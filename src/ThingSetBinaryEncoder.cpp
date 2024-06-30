/*
 * Copyright (c) 2024 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */
#include "ThingSetBinaryEncoder.hpp"

bool ThingSet::ThingSetBinaryEncoder::encode(const std::string_view &value)
{
    return encode(std::string(value));
}

bool ThingSet::ThingSetBinaryEncoder::encode(std::string_view &value)
{
    return encode(std::string(value));
}

bool ThingSet::ThingSetBinaryEncoder::encode(const std::string &value)
{
    return encode(value.c_str());
}

bool ThingSet::ThingSetBinaryEncoder::encode(std::string &value)
{
    return encode(value.c_str());
}

bool ThingSet::ThingSetBinaryEncoder::encode(const char *value)
{
#ifdef zcbor_tstr_put_term
    return zcbor_tstr_put_term(this->getEncoder(), value);
#else
    return zcbor_tstr_put_term(this->getState(), value, BINARY_ENCODER_MAX_NULL_TERMINATED_STRING_LENGTH);
#endif
}

bool ThingSet::ThingSetBinaryEncoder::encode(char *value)
{
#ifdef zcbor_tstr_put_term
    return zcbor_tstr_put_term(this->getEncoder(), value);
#else
    return zcbor_tstr_put_term(this->getState(), value, BINARY_ENCODER_MAX_NULL_TERMINATED_STRING_LENGTH);
#endif
}

bool ThingSet::ThingSetBinaryEncoder::encode(const float &value)
{
    return zcbor_float32_put(this->getState(), value);
}

bool ThingSet::ThingSetBinaryEncoder::encode(float &value)
{
    return zcbor_float32_put(this->getState(), value);
}

bool ThingSet::ThingSetBinaryEncoder::encode(const double &value)
{
    return zcbor_float64_put(this->getState(), value);
}

bool ThingSet::ThingSetBinaryEncoder::encode(double &value)
{
    return zcbor_float64_put(this->getState(), value);
}

bool ThingSet::ThingSetBinaryEncoder::encode(const bool &value)
{
    return zcbor_bool_put(this->getState(), value);
}

bool ThingSet::ThingSetBinaryEncoder::encode(bool &value)
{
    return zcbor_bool_put(this->getState(), value);
}

bool ThingSet::ThingSetBinaryEncoder::encode(const uint8_t &value)
{
    return zcbor_uint32_put(this->getState(), value);
}

bool ThingSet::ThingSetBinaryEncoder::encode(uint8_t &value)
{
    return zcbor_uint32_put(this->getState(), value);
}

bool ThingSet::ThingSetBinaryEncoder::encode(const uint16_t &value)
{
    return zcbor_uint32_put(this->getState(), value);
}

bool ThingSet::ThingSetBinaryEncoder::encode(uint16_t &value)
{
    return zcbor_uint32_put(this->getState(), value);
}

bool ThingSet::ThingSetBinaryEncoder::encode(const uint32_t &value)
{
    return zcbor_uint32_put(this->getState(), value);
}

bool ThingSet::ThingSetBinaryEncoder::encode(uint32_t &value)
{
    return zcbor_uint32_put(this->getState(), value);
}

bool ThingSet::ThingSetBinaryEncoder::encode(const uint64_t &value)
{
    return zcbor_uint64_put(this->getState(), value);
}

bool ThingSet::ThingSetBinaryEncoder::encode(uint64_t &value)
{
    return zcbor_uint64_put(this->getState(), value);
}

bool ThingSet::ThingSetBinaryEncoder::encode(const int8_t &value)
{
    return zcbor_int32_put(this->getState(), value);
}

bool ThingSet::ThingSetBinaryEncoder::encode(int8_t &value)
{
    return zcbor_int32_put(this->getState(), value);
}

bool ThingSet::ThingSetBinaryEncoder::encode(const int16_t &value)
{
    return zcbor_int32_put(this->getState(), value);
}

bool ThingSet::ThingSetBinaryEncoder::encode(int16_t &value)
{
    return zcbor_int32_put(this->getState(), value);
}

bool ThingSet::ThingSetBinaryEncoder::encode(const int32_t &value)
{
    return zcbor_int32_put(this->getState(), value);
}

bool ThingSet::ThingSetBinaryEncoder::encode(int32_t &value)
{
    return zcbor_int32_put(this->getState(), value);
}

bool ThingSet::ThingSetBinaryEncoder::encode(const int64_t &value)
{
    return zcbor_int64_put(this->getState(), value);
}

bool ThingSet::ThingSetBinaryEncoder::encode(int64_t &value)
{
    return zcbor_int64_put(this->getState(), value);
}

bool ThingSet::ThingSetBinaryEncoder::encodeNull()
{
    return zcbor_nil_put(this->getState(), NULL);
}

bool ThingSet::ThingSetBinaryEncoder::encodeListStart()
{
    return zcbor_list_start_encode(this->getState(), UINT8_MAX);
}

bool ThingSet::ThingSetBinaryEncoder::encodeListEnd()
{
    return zcbor_list_end_encode(this->getState(), UINT8_MAX);
}

bool ThingSet::ThingSetBinaryEncoder::encodeMapStart()
{
    return zcbor_map_start_encode(this->getState(), UINT8_MAX);
}

bool ThingSet::ThingSetBinaryEncoder::encodeMapEnd()
{
    return zcbor_map_end_encode(this->getState(), UINT8_MAX);
}
