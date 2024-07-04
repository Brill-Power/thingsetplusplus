/*
 * Copyright (c) 2024 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */
#include "ThingSetBinaryEncoder.hpp"

namespace ThingSet {

static bool encodeListMapEnd(zcbor_state_t *state);

const bool ThingSetBinaryEncoder::getIsForwardOnly() const
{
    return false;
}

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
    return zcbor_tstr_put_term(this->getState(), value);
#else
    return zcbor_tstr_put_term(this->getState(), value, BINARY_ENCODER_MAX_NULL_TERMINATED_STRING_LENGTH);
#endif
}

bool ThingSetBinaryEncoder::encode(char *value)
{
#ifdef zcbor_tstr_put_term
    return zcbor_tstr_put_term(this->getState(), value);
#else
    return zcbor_tstr_put_term(this->getState(), value, BINARY_ENCODER_MAX_NULL_TERMINATED_STRING_LENGTH);
#endif
}

bool ThingSetBinaryEncoder::encode(const float &value)
{
    return zcbor_float32_put(this->getState(), value);
}

bool ThingSetBinaryEncoder::encode(float &value)
{
    return zcbor_float32_put(this->getState(), value);
}

bool ThingSetBinaryEncoder::encode(const double &value)
{
    return zcbor_float64_put(this->getState(), value);
}

bool ThingSetBinaryEncoder::encode(double &value)
{
    return zcbor_float64_put(this->getState(), value);
}

bool ThingSetBinaryEncoder::encode(const bool &value)
{
    return zcbor_bool_put(this->getState(), value);
}

bool ThingSetBinaryEncoder::encode(bool &value)
{
    return zcbor_bool_put(this->getState(), value);
}

bool ThingSetBinaryEncoder::encode(const uint8_t &value)
{
    return zcbor_uint32_put(this->getState(), value);
}

bool ThingSetBinaryEncoder::encode(uint8_t &value)
{
    return zcbor_uint32_put(this->getState(), value);
}

bool ThingSetBinaryEncoder::encode(const uint16_t &value)
{
    return zcbor_uint32_put(this->getState(), value);
}

bool ThingSetBinaryEncoder::encode(uint16_t &value)
{
    return zcbor_uint32_put(this->getState(), value);
}

bool ThingSetBinaryEncoder::encode(const uint32_t &value)
{
    return zcbor_uint32_put(this->getState(), value);
}

bool ThingSetBinaryEncoder::encode(uint32_t &value)
{
    return zcbor_uint32_put(this->getState(), value);
}

bool ThingSetBinaryEncoder::encode(const uint64_t &value)
{
    return zcbor_uint64_put(this->getState(), value);
}

bool ThingSetBinaryEncoder::encode(uint64_t &value)
{
    return zcbor_uint64_put(this->getState(), value);
}

bool ThingSetBinaryEncoder::encode(const int8_t &value)
{
    return zcbor_int32_put(this->getState(), value);
}

bool ThingSetBinaryEncoder::encode(int8_t &value)
{
    return zcbor_int32_put(this->getState(), value);
}

bool ThingSetBinaryEncoder::encode(const int16_t &value)
{
    return zcbor_int32_put(this->getState(), value);
}

bool ThingSetBinaryEncoder::encode(int16_t &value)
{
    return zcbor_int32_put(this->getState(), value);
}

bool ThingSetBinaryEncoder::encode(const int32_t &value)
{
    return zcbor_int32_put(this->getState(), value);
}

bool ThingSetBinaryEncoder::encode(int32_t &value)
{
    return zcbor_int32_put(this->getState(), value);
}

bool ThingSetBinaryEncoder::encode(const int64_t &value)
{
    return zcbor_int64_put(this->getState(), value);
}

bool ThingSetBinaryEncoder::encode(int64_t &value)
{
    return zcbor_int64_put(this->getState(), value);
}

bool ThingSetBinaryEncoder::encodeNull()
{
    return zcbor_nil_put(this->getState(), NULL);
}

bool ThingSetBinaryEncoder::encodeListStart()
{
    return encodeListStart(UINT8_MAX);
}

bool ThingSetBinaryEncoder::encodeListStart(uint32_t count)
{
    return zcbor_list_start_encode(this->getState(), count);
}

bool ThingSetBinaryEncoder::encodeListEnd()
{
    return encodeListEnd(UINT8_MAX);
}

bool ThingSetBinaryEncoder::encodeListEnd(uint32_t count)
{
    if (this->getIsForwardOnly()) {
        return encodeListMapEnd(this->getState());
    }
    else {
        return zcbor_list_end_encode(this->getState(), count);
    }
}

bool ThingSetBinaryEncoder::encodeMapStart()
{
    return encodeMapStart(UINT8_MAX);
}

bool ThingSetBinaryEncoder::encodeMapStart(uint32_t count)
{
    return zcbor_map_start_encode(this->getState(), count);
}

bool ThingSetBinaryEncoder::encodeMapEnd()
{
    return encodeMapEnd(UINT8_MAX);
}

bool ThingSetBinaryEncoder::encodeMapEnd(uint32_t count)
{
    if (this->getIsForwardOnly()) {
        return encodeListMapEnd(this->getState());
    }
    else {
        return zcbor_map_end_encode(this->getState(), count);
    }
}

/// @brief End a list or map without going back and rewriting the header. Useful in forward-only encoding scenarios.
/// @param state The encoder state array.
/// @return True.
static bool encodeListMapEnd(zcbor_state_t *state)
{
    // zcbor_list_map_end_force_encode is broken; it resets the pointer
    // back to where the list started, rendering it pointless, so we reimplement
    // it here, passing in the necessary TRANSFER_PAYLOAD flag to make it behave
    // properly
    if (!zcbor_process_backup(state, ZCBOR_FLAG_RESTORE | ZCBOR_FLAG_CONSUME | ZCBOR_FLAG_TRANSFER_PAYLOAD,
                              ZCBOR_MAX_ELEM_COUNT))
    {
        ZCBOR_FAIL();
    }
    return true;
}

} // namespace ThingSet