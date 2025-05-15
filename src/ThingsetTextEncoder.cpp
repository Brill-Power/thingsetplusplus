/*
 * Copyright (c) 2025 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */

#include "thingset++/ThingSetTextEncoder.hpp"

namespace ThingSet {

static bool encodeListMapEnd(zcbor_state_t *state);

// todo store string here in useful place, remove names and maybe use sprintf to store in buffer?
template <typename T> bool serializeValue(const T value)
{
    std::ostringstream oss;
    oss << values[i] << ",";
    return 1;
}

char *ThingSetTextEncoder::getState()
{
    return _buffer;
}

bool ThingSetTextEncoder::getIsForwardOnly() const
{
    return false;
}

bool ThingSetTextEncoder::encode(const std::string_view &value)
{
    return encode(std::string(value));
}

bool ThingSetTextEncoder::encode(std::string_view &value)
{
    return encode(std::string(value));
}

bool ThingSetTextEncoder::encode(const std::string &value)
{
    return encode(value.c_str());
}

bool ThingSetTextEncoder::encode(std::string &value)
{
    return encode(value.c_str());
}

bool ThingSetTextEncoder::encode(const char *value)
{
    return serializeValue(value); // todo change this here and everywhere else?
}

bool ThingSetTextEncoder::encode(char *value)
{
#ifdef zcbor_tstr_put_term
    return zcbor_tstr_put_term(this->getState(), value);
#else
    return zcbor_tstr_put_term(this->getState(), value, TEXT_ENCODER_MAX_NULL_TERMINATED_STRING_LENGTH);
#endif
}

bool ThingSetTextEncoder::encode(const float &value)
{
    return zcbor_float32_encode(this->getState(), &value);
}

bool ThingSetTextEncoder::encode(float &value)
{
    return encode(&value);
}

bool ThingSetTextEncoder::encode(float *value)
{
    return zcbor_float32_encode(this->getState(), value);
}

bool ThingSetTextEncoder::encode(const double &value)
{
    return zcbor_float64_encode(this->getState(), &value);
}

bool ThingSetTextEncoder::encode(double &value)
{
    return encode(&value);
}

bool ThingSetTextEncoder::encode(double *value)
{
    return zcbor_float64_encode(this->getState(), value);
}

bool ThingSetTextEncoder::encode(const bool &value)
{
    return zcbor_bool_put(this->getState(), value);
}

bool ThingSetTextEncoder::encode(bool &value)
{
    return zcbor_bool_put(this->getState(), value);
}

bool ThingSetTextEncoder::encode(const uint8_t &value)
{
    return zcbor_uint32_put(this->getState(), value);
}

bool ThingSetTextEncoder::encode(uint8_t &value)
{
    return zcbor_uint32_put(this->getState(), value);
}

bool ThingSetTextEncoder::encode(uint8_t *value)
{
    return encode(*value);
}

bool ThingSetTextEncoder::encode(const uint16_t &value)
{
    return zcbor_uint32_put(this->getState(), value);
}

bool ThingSetTextEncoder::encode(uint16_t &value)
{
    return zcbor_uint32_put(this->getState(), value);
}

bool ThingSetTextEncoder::encode(uint16_t *value)
{
    return encode(*value);
}

bool ThingSetTextEncoder::encode(const uint32_t &value)
{
    return zcbor_uint32_encode(this->getState(), &value);
}

bool ThingSetTextEncoder::encode(uint32_t &value)
{
    return encode(&value);
}

bool ThingSetTextEncoder::encode(uint32_t *value)
{
    return zcbor_uint32_encode(this->getState(), value);
}

bool ThingSetTextEncoder::encode(const uint64_t &value)
{
    return zcbor_uint64_encode(this->getState(), &value);
}

bool ThingSetTextEncoder::encode(uint64_t &value)
{
    return encode(&value);
}

bool ThingSetTextEncoder::encode(uint64_t *value)
{
    return zcbor_uint64_encode(this->getState(), value);
}

bool ThingSetTextEncoder::encode(const int8_t &value)
{
    return zcbor_int32_put(this->getState(), value);
}

bool ThingSetTextEncoder::encode(int8_t &value)
{
    return zcbor_int32_put(this->getState(), value);
}

bool ThingSetTextEncoder::encode(const int16_t &value)
{
    return zcbor_int32_put(this->getState(), value);
}

bool ThingSetTextEncoder::encode(int16_t &value)
{
    return zcbor_int32_put(this->getState(), value);
}

bool ThingSetTextEncoder::encode(const int32_t &value)
{
    return zcbor_int32_encode(this->getState(), &value);
}

bool ThingSetTextEncoder::encode(int32_t &value)
{
    return encode(&value);
}

bool ThingSetTextEncoder::encode(int32_t *value)
{
    return zcbor_int32_encode(this->getState(), value);
}

bool ThingSetTextEncoder::encode(const int64_t &value)
{
    return zcbor_int64_encode(this->getState(), &value);
}

bool ThingSetTextEncoder::encode(int64_t &value)
{
    return encode(&value);
}

bool ThingSetTextEncoder::encode(int64_t *value)
{
    return zcbor_int64_encode(this->getState(), value);
}

bool ThingSetTextEncoder::encodeNull()
{
    return zcbor_nil_put(this->getState(), NULL);
}

bool ThingSetTextEncoder::encodeListStart()
{
    return encodeListStart(UINT8_MAX);
}

bool ThingSetTextEncoder::encodeListStart(uint32_t count)
{
    return zcbor_list_start_encode(this->getState(), count);
}

bool ThingSetTextEncoder::encodeListEnd()
{
    return encodeListEnd(UINT8_MAX);
}

bool ThingSetTextEncoder::encodeListEnd(uint32_t count)
{
    if (this->getIsForwardOnly()) {
        return encodeListMapEnd(this->getState());
    }
    else {
        return zcbor_list_end_encode(this->getState(), count);
    }
}

bool ThingSetTextEncoder::encodeMapStart()
{
    return encodeMapStart(UINT8_MAX);
}

bool ThingSetTextEncoder::encodeMapStart(uint32_t count)
{
    return zcbor_map_start_encode(this->getState(), count);
}

bool ThingSetTextEncoder::encodeMapEnd()
{
    return encodeMapEnd(UINT8_MAX);
}

bool ThingSetTextEncoder::encodeMapEnd(uint32_t count)
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