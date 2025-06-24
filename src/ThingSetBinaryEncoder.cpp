/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "thingset++/ThingSetBinaryEncoder.hpp"

namespace ThingSet {

static bool encodeListMapEnd(zcbor_state_t *state);

bool ThingSetBinaryEncoder::ensureState()
{
    return true;
}

bool ThingSetBinaryEncoder::getIsForwardOnly() const
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
    return this->ensureState() && zcbor_tstr_put_term(this->getState(), value);
#else
    return this->ensureState()
           && zcbor_tstr_put_term(this->getState(), value, BINARY_ENCODER_MAX_NULL_TERMINATED_STRING_LENGTH);
#endif
}

bool ThingSetBinaryEncoder::encode(char *value)
{
#ifdef zcbor_tstr_put_term
    return this->ensureState() && zcbor_tstr_put_term(this->getState(), value);
#else
    return this->ensureState()
           && zcbor_tstr_put_term(this->getState(), value, BINARY_ENCODER_MAX_NULL_TERMINATED_STRING_LENGTH);
#endif
}

bool ThingSetBinaryEncoder::encode(const float &value)
{
    return this->ensureState() && zcbor_float32_encode(this->getState(), &value);
}

bool ThingSetBinaryEncoder::encode(float &value)
{
    return encode(&value);
}

bool ThingSetBinaryEncoder::encode(float *value)
{
    return this->ensureState() && zcbor_float32_encode(this->getState(), value);
}

bool ThingSetBinaryEncoder::encode(const double &value)
{
    return this->ensureState() && zcbor_float64_encode(this->getState(), &value);
}

bool ThingSetBinaryEncoder::encode(double &value)
{
    return encode(&value);
}

bool ThingSetBinaryEncoder::encode(double *value)
{
    return this->ensureState() && zcbor_float64_encode(this->getState(), value);
}

bool ThingSetBinaryEncoder::encode(const bool &value)
{
    return this->ensureState() && zcbor_bool_put(this->getState(), value);
}

bool ThingSetBinaryEncoder::encode(bool &value)
{
    return this->ensureState() && zcbor_bool_put(this->getState(), value);
}

bool ThingSetBinaryEncoder::encode(const uint8_t &value)
{
    return this->ensureState() && zcbor_uint32_put(this->getState(), value);
}

bool ThingSetBinaryEncoder::encode(uint8_t &value)
{
    return this->ensureState() && zcbor_uint32_put(this->getState(), value);
}

bool ThingSetBinaryEncoder::encode(uint8_t *value)
{
    return encode(*value);
}

bool ThingSetBinaryEncoder::encode(const uint16_t &value)
{
    return this->ensureState() && zcbor_uint32_put(this->getState(), value);
}

bool ThingSetBinaryEncoder::encode(uint16_t &value)
{
    return this->ensureState() && zcbor_uint32_put(this->getState(), value);
}

bool ThingSetBinaryEncoder::encode(uint16_t *value)
{
    return encode(*value);
}

bool ThingSetBinaryEncoder::encode(const uint32_t &value)
{
    return this->ensureState() && zcbor_uint32_encode(this->getState(), &value);
}

bool ThingSetBinaryEncoder::encode(uint32_t &value)
{
    return encode(&value);
}

bool ThingSetBinaryEncoder::encode(uint32_t *value)
{
    return this->ensureState() && zcbor_uint32_encode(this->getState(), value);
}

bool ThingSetBinaryEncoder::encode(const uint64_t &value)
{
    return this->ensureState() && zcbor_uint64_encode(this->getState(), &value);
}

bool ThingSetBinaryEncoder::encode(uint64_t &value)
{
    return encode(&value);
}

bool ThingSetBinaryEncoder::encode(uint64_t *value)
{
    return this->ensureState() && zcbor_uint64_encode(this->getState(), value);
}

bool ThingSetBinaryEncoder::encode(const int8_t &value)
{
    return this->ensureState() && zcbor_int32_put(this->getState(), value);
}

bool ThingSetBinaryEncoder::encode(int8_t &value)
{
    return this->ensureState() && zcbor_int32_put(this->getState(), value);
}

bool ThingSetBinaryEncoder::encode(const int16_t &value)
{
    return this->ensureState() && zcbor_int32_put(this->getState(), value);
}

bool ThingSetBinaryEncoder::encode(int16_t &value)
{
    return this->ensureState() && zcbor_int32_put(this->getState(), value);
}

bool ThingSetBinaryEncoder::encode(const int32_t &value)
{
    return this->ensureState() && zcbor_int32_encode(this->getState(), &value);
}

bool ThingSetBinaryEncoder::encode(int32_t &value)
{
    return encode(&value);
}

bool ThingSetBinaryEncoder::encode(int32_t *value)
{
    return this->ensureState() && zcbor_int32_encode(this->getState(), value);
}

bool ThingSetBinaryEncoder::encode(const int64_t &value)
{
    return this->ensureState() && zcbor_int64_encode(this->getState(), &value);
}

bool ThingSetBinaryEncoder::encode(int64_t &value)
{
    return encode(&value);
}

bool ThingSetBinaryEncoder::encode(int64_t *value)
{
    return this->ensureState() && zcbor_int64_encode(this->getState(), value);
}

bool ThingSetBinaryEncoder::encodeNull()
{
    return this->ensureState() && zcbor_nil_put(this->getState(), NULL);
}

bool ThingSetBinaryEncoder::encodePreamble()
{
    return encodeNull();
}

bool ThingSetBinaryEncoder::encodeListStart()
{
    return encodeListStart(UINT8_MAX);
}

bool ThingSetBinaryEncoder::encodeListStart(const uint32_t &count)
{
    return this->ensureState() && zcbor_list_start_encode(this->getState(), count);
}

bool ThingSetBinaryEncoder::encodeListEnd()
{
    return encodeListEnd(UINT8_MAX);
}

bool ThingSetBinaryEncoder::encodeListEnd(const uint32_t &count)
{
    if (this->getIsForwardOnly()) {
        return this->ensureState() && encodeListMapEnd(this->getState());
    }
    else {
        return this->ensureState() && zcbor_list_end_encode(this->getState(), count);
    }
}

bool ThingSetBinaryEncoder::encodeMapStart()
{
    return encodeMapStart(UINT8_MAX);
}

bool ThingSetBinaryEncoder::encodeMapStart(const uint32_t &count)
{
    return this->ensureState() && zcbor_map_start_encode(this->getState(), count);
}

bool ThingSetBinaryEncoder::encodeMapEnd()
{
    return encodeMapEnd(UINT8_MAX);
}

bool ThingSetBinaryEncoder::encodeMapEnd(const uint32_t &count)
{
    if (this->getIsForwardOnly()) {
        return this->ensureState() && encodeListMapEnd(this->getState());
    }
    else {
        return this->ensureState() && zcbor_map_end_encode(this->getState(), count);
    }
}

bool ThingSetBinaryEncoder::encodeListSeparator()
{
    return true;
}

bool ThingSetBinaryEncoder::encodeKeyValuePairSeparator()
{
    return true;
}

/// @brief End a list or map without going back and rewriting the header. Useful in forward-only encoding scenarios.
/// @param state The encoder state array.
/// @return True.
static bool encodeListMapEnd(zcbor_state_t *state)
{
    // zcbor_list_map_end_force_encode is broken; it resets the pointer
    // back to where the list started, rendering it pointless, so we reimplement
    // it here, passing in the necessary TRANSFER/KEEP_PAYLOAD flag to make it behave
    // properly
    auto flags = ZCBOR_FLAG_RESTORE | ZCBOR_FLAG_CONSUME;
#ifdef ZCBOR_FLAG_TRANSFER_PAYLOAD
    flags |= ZCBOR_FLAG_TRANSFER_PAYLOAD;
#else
    flags |= ZCBOR_FLAG_KEEP_PAYLOAD;
#endif
    return zcbor_process_backup(state, flags, ZCBOR_MAX_ELEM_COUNT);
}

} // namespace ThingSet