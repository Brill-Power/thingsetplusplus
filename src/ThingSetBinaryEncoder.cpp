/*
 * Copyright (c) 2024 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */
#include "ThingSetBinaryEncoder.hpp"

namespace ThingSet {

bool ThingSetBinaryEncoder::encode(const std::string_view &value)
{
    return encode(std::string(value));
}

bool ThingSetBinaryEncoder::encode(std::string_view &value)
{
    return encode(std::string(value));
}

bool ThingSetBinaryEncoder::encode(const std::string &value)
{
    return encode(value.c_str());
}

bool ThingSetBinaryEncoder::encode(std::string &value)
{
    return encode(value.c_str());
}

bool ThingSetBinaryEncoder::encode(const char *value)
{
#ifdef zcbor_tstr_put_term
    return zcbor_tstr_put_term(this->getState(BINARY_ENCODER_MAX_NULL_TERMINATED_STRING_LENGTH), value);
#else
    return zcbor_tstr_put_term(this->getState(BINARY_ENCODER_MAX_NULL_TERMINATED_STRING_LENGTH), value,
                               BINARY_ENCODER_MAX_NULL_TERMINATED_STRING_LENGTH);
#endif
}

bool ThingSetBinaryEncoder::encode(char *value)
{
#ifdef zcbor_tstr_put_term
    return zcbor_tstr_put_term(this->getState(BINARY_ENCODER_MAX_NULL_TERMINATED_STRING_LENGTH), value);
#else
    return zcbor_tstr_put_term(this->getState(BINARY_ENCODER_MAX_NULL_TERMINATED_STRING_LENGTH), value,
                               BINARY_ENCODER_MAX_NULL_TERMINATED_STRING_LENGTH);
#endif
}

bool ThingSetBinaryEncoder::encode(const float &value)
{
    return zcbor_float32_put(this->getState(1 + sizeof(float)), value);
}

bool ThingSetBinaryEncoder::encode(float &value)
{
    return zcbor_float32_put(this->getState(1 + sizeof(float)), value);
}

bool ThingSetBinaryEncoder::encode(const double &value)
{
    return zcbor_float64_put(this->getState(1 + sizeof(double)), value);
}

bool ThingSetBinaryEncoder::encode(double &value)
{
    return zcbor_float64_put(this->getState(1 + sizeof(double)), value);
}

bool ThingSetBinaryEncoder::encode(const bool &value)
{
    return zcbor_bool_put(this->getState(1), value);
}

bool ThingSetBinaryEncoder::encode(bool &value)
{
    return zcbor_bool_put(this->getState(1), value);
}

bool ThingSetBinaryEncoder::encode(const uint8_t &value)
{
    return zcbor_uint32_put(this->getState(1 + sizeof(uint8_t)), value);
}

bool ThingSetBinaryEncoder::encode(uint8_t &value)
{
    return zcbor_uint32_put(this->getState(1 + sizeof(uint8_t)), value);
}

bool ThingSetBinaryEncoder::encode(const uint16_t &value)
{
    return zcbor_uint32_put(this->getState(1 + sizeof(uint16_t)), value);
}

bool ThingSetBinaryEncoder::encode(uint16_t &value)
{
    return zcbor_uint32_put(this->getState(1 + sizeof(uint16_t)), value);
}

bool ThingSetBinaryEncoder::encode(const uint32_t &value)
{
    return zcbor_uint32_put(this->getState(1 + sizeof(uint32_t)), value);
}

bool ThingSetBinaryEncoder::encode(uint32_t &value)
{
    return zcbor_uint32_put(this->getState(1 + sizeof(uint32_t)), value);
}

bool ThingSetBinaryEncoder::encode(const uint64_t &value)
{
    return zcbor_uint64_put(this->getState(1 + sizeof(uint64_t)), value);
}

bool ThingSetBinaryEncoder::encode(uint64_t &value)
{
    return zcbor_uint64_put(this->getState(1 + sizeof(uint64_t)), value);
}

bool ThingSetBinaryEncoder::encode(const int8_t &value)
{
    return zcbor_int32_put(this->getState(1 + sizeof(int8_t)), value);
}

bool ThingSetBinaryEncoder::encode(int8_t &value)
{
    return zcbor_int32_put(this->getState(1 + sizeof(int8_t)), value);
}

bool ThingSetBinaryEncoder::encode(const int16_t &value)
{
    return zcbor_int32_put(this->getState(1 + sizeof(int16_t)), value);
}

bool ThingSetBinaryEncoder::encode(int16_t &value)
{
    return zcbor_int32_put(this->getState(1 + sizeof(int16_t)), value);
}

bool ThingSetBinaryEncoder::encode(const int32_t &value)
{
    return zcbor_int32_put(this->getState(1 + sizeof(int32_t)), value);
}

bool ThingSetBinaryEncoder::encode(int32_t &value)
{
    return zcbor_int32_put(this->getState(1 + sizeof(int32_t)), value);
}

bool ThingSetBinaryEncoder::encode(const int64_t &value)
{
    return zcbor_int64_put(this->getState(1 + sizeof(int64_t)), value);
}

bool ThingSetBinaryEncoder::encode(int64_t &value)
{
    return zcbor_int64_put(this->getState(1 + sizeof(int64_t)), value);
}

bool ThingSetBinaryEncoder::encodeNull()
{
    return zcbor_nil_put(this->getState(1), NULL);
}

bool ThingSetBinaryEncoder::encodeListStart()
{
    return encodeListStart(UINT8_MAX);
}

bool ThingSetBinaryEncoder::encodeListStart(uint32_t count)
{
    // TODO: this would be a massive overestimate if this is called with e.g. UINT8_MAX
    return zcbor_list_start_encode(this->getState((count + 1) * 5), count);
}

bool ThingSetBinaryEncoder::encodeListEnd()
{
    return encodeListEnd(UINT8_MAX);
}

bool ThingSetBinaryEncoder::encodeListEnd(uint32_t count)
{
    return zcbor_list_end_encode(this->getState(0), count);
}

bool ThingSetBinaryEncoder::encodeMapStart()
{
    return encodeMapStart(UINT8_MAX);
}

bool ThingSetBinaryEncoder::encodeMapStart(uint32_t count)
{
    return zcbor_map_start_encode(this->getState(count * 7), count);
}

bool ThingSetBinaryEncoder::encodeMapEnd()
{
    return encodeMapEnd(UINT8_MAX);
}

bool ThingSetBinaryEncoder::encodeMapEnd(uint32_t count)
{
    return zcbor_map_end_encode(this->getState(0), count);
}

} // namespace ThingSet