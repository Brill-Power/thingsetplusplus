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

void ThingSetBinaryDecoder::initialiseState(zcbor_state_t *state, const size_t depth, const uint8_t *buffer, const size_t size, const int elementCount)
{
#ifdef zcbor_tstr_expect_term
        zcbor_new_decode_state(_state, depth, buffer, size, elementCount);
#else
        zcbor_new_decode_state(state, depth, buffer, size, elementCount, NULL, 0);
#ifdef ZCBOR_ENFORCE_CANONICAL
        state->constant_state->enforce_canonical = false;
#endif // ZCBOR_ENFORCE_CANONICAL
#endif // zcbor_tstr_expect_term
}

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

bool ThingSetBinaryDecoder::decodeBytes(uint8_t *buffer, size_t capacity, size_t &size)
{
    zcbor_string result;
    if (zcbor_bstr_decode(getState(), &result) && result.len <= capacity) {
        memcpy(buffer, result.value, result.len);
        size = result.len;
        return true;
    }
    return false;
}

bool ThingSetBinaryDecoder::decodeMapStart()
{
    return zcbor_map_start_decode(getState());
}

bool ThingSetBinaryDecoder::decodeMapEnd()
{
    return zcbor_map_end_decode(getState());
}

bool ThingSetBinaryDecoder::isInMap() const
{
    return getState()->elem_count != 0;
}

bool ThingSetBinaryDecoder::isInList() const
{
    return getState()->elem_count != 0;
}

bool ThingSetBinaryDecoder::ensureListSize(const size_t size, size_t &elementCount)
{
    // check if number of elements in stream matches array size
    elementCount = getState()->elem_count;
    if (size > elementCount && (_options & ThingSetBinaryDecoderOptions::allowUndersizedArrays) == 0) {
        if (!getIsForwardOnly()) {
            // wind the state back to before we started parsing the list, so that a
            // call to `skip()` will skip the whole array
            zcbor_process_backup(getState(), ZCBOR_FLAG_RESTORE | ZCBOR_FLAG_CONSUME, ZCBOR_MAX_ELEM_COUNT);
            // because the backup is taken *after* it has consumed the byte(s) containing the number
            // of elements in the array, this restore doesn't really work
            // so we overwrite the payload pointer with the backup it takes
            getState()->payload = getState()->payload_bak;
            // we also need to increment this, because, yes, it has consumed this as well
            getState()->elem_count++;
        }
        return false;
    }
    return true;
}

ThingSetEncodedNodeType ThingSetBinaryDecoder::peekType()
{
    zcbor_major_type_t type = ZCBOR_MAJOR_TYPE(this->getState()->payload[0]);
    switch (type) {
        case ZCBOR_MAJOR_TYPE_LIST:
            return ThingSetEncodedNodeType::list;
        case ZCBOR_MAJOR_TYPE_MAP:
            return ThingSetEncodedNodeType::map;
        case ZCBOR_MAJOR_TYPE_NINT:
        case ZCBOR_MAJOR_TYPE_PINT:
        case ZCBOR_MAJOR_TYPE_SIMPLE:
            return ThingSetEncodedNodeType::primitive;
        case ZCBOR_MAJOR_TYPE_BSTR:
        case ZCBOR_MAJOR_TYPE_TSTR:
            return ThingSetEncodedNodeType::string;
        default:
            return ThingSetEncodedNodeType::unknown;
    }
}

bool ThingSetBinaryDecoder::skip()
{
    return zcbor_any_skip(this->getState(), NULL);
}

} // namespace ThingSet