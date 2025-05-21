/*
 * Copyright (c) 2025 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */

#include "thingset++/ThingSetTextEncoder.hpp"
#include "inttypes.h"

namespace ThingSet {

// todo may not be needed?
// inline bool encodeStart(struct thingset_context *ts, char c)
// {
//     if (ts->rsp_size > ts->rsp_pos + 2) {
//         ts->rsp[ts->rsp_pos++] = c;
//         return true;
//     }
//     else {
//         ts->rsp_pos = 0;
//         return false;
//     }
// }

// inline bool encodeEnd(struct thingset_context *ts, char c)
// {
//     if (ts->rsp_size > ts->rsp_pos + 3) {
//         if (ts->rsp[ts->rsp_pos - 1] == ',') {
//             ts->rsp_pos--;
//         }
//         ts->rsp[ts->rsp_pos++] = c;
//         ts->rsp[ts->rsp_pos++] = ',';
//         return true;
//     }
//     else {
//         ts->rsp_pos = 0;
//         return false;
//     }
// }

// encodes a value of a simple type using a format specifier (eg %i, %f etc), convert this to work in this application
template <typename T> inline bool encodeSimpleValue(struct thingset_context *ts, T value, const char *format)
{
    // todo move this to a getFutureEncodedLength in header file?
    int bytes_required = snprintf(nullptr, 0, format, value); // determine the length of value as a string

    if (sizeof(ts->rsp) < ts->rsp_pos + bytes_required) {
        return false;
    }

    // todo use getEncodedLength instead of sizeof section here?
    int bytes_written = snprintf(ts->rsp + ts->rsp_pos, sizeof(ts->rsp) - ts->rsp_pos, format, value);
    ts->rsp_pos += bytes_written;

    return true;
}

// todo may not be needed?
// char *ThingSetTextEncoder::getState()
// {
//     return _buffer;
// }

// bool ThingSetTextEncoder::getIsForwardOnly() const
// {
//     return false;
// }

bool ThingSetTextEncoder::encode(const std::string_view &value)
{
    return encodesimplevalue(ts, value, "%.*s");
}

bool ThingSetTextEncoder::encode(std::string_view &value)
{
    return encodesimplevalue(ts, value, "%.*s");
}

bool ThingSetTextEncoder::encode(const std::string &value)
{
    return encodesimplevalue(ts, value.c_str(), "%s");
}

bool ThingSetTextEncoder::encode(std::string &value)
{
    return encodesimplevalue(ts, value.c_str(), "%s");
}

bool ThingSetTextEncoder::encode(const char *value)
{
    return encodesimplevalue(ts, value, "%s");
}

bool ThingSetTextEncoder::encode(char *value)
{
    return encodesimplevalue(ts, value, "%s");
}

bool ThingSetTextEncoder::encode(const float &value)
{
    return encodesimplevalue(ts, value, "%f");
}

bool ThingSetTextEncoder::encode(float &value)
{
    return encodesimplevalue(ts, value, "%f");
}

bool ThingSetTextEncoder::encode(float *value)
{
    return encodesimplevalue(ts, value, "%f");
}

bool ThingSetTextEncoder::encode(const double &value)
{
    return encodesimplevalue(ts, value, "%f");
}

bool ThingSetTextEncoder::encode(double &value)
{
    return encodesimplevalue(ts, value, "%f");
}

bool ThingSetTextEncoder::encode(double *value)
{
    return encodesimplevalue(ts, value, "%f");
}

// todo bool may not be needed as handled by int (1/0 instead of true/false)
// bool ThingSetTextEncoder::encode(const bool &value)
// {
//     return zcbor_bool_put(this->getState(), value);
// }

// bool ThingSetTextEncoder::encode(bool &value)
// {
//     return zcbor_bool_put(this->getState(), value);
// }

bool ThingSetTextEncoder::encode(const uint8_t &value)
{
    return encodesimplevalue(ts, value, "%" PRIu8);
}

bool ThingSetTextEncoder::encode(uint8_t &value)
{
    return encodesimplevalue(ts, value, "%" PRIu8);
}

bool ThingSetTextEncoder::encode(uint8_t *value)
{
    return encodesimplevalue(ts, value, "%" PRIu8);
}

bool ThingSetTextEncoder::encode(const uint16_t &value)
{
    return encodesimplevalue(ts, value, "%" PRIu16);
}

bool ThingSetTextEncoder::encode(uint16_t &value)
{
    return encodesimplevalue(ts, value, "%" PRIu16);
}

bool ThingSetTextEncoder::encode(uint16_t *value)
{
    return encodesimplevalue(ts, value, "%" PRIu16);
}

bool ThingSetTextEncoder::encode(const uint32_t &value)
{
    return encodesimplevalue(ts, value, "%" PRIu32);
}

bool ThingSetTextEncoder::encode(uint32_t &value)
{
    return encodesimplevalue(ts, value, "%" PRIu32);
}

bool ThingSetTextEncoder::encode(uint32_t *value)
{
    return encodesimplevalue(ts, value, "%" PRIu32);
}

bool ThingSetTextEncoder::encode(const uint64_t &value)
{
    return encodesimplevalue(ts, value, "%" PRIu64);
}

bool ThingSetTextEncoder::encode(uint64_t &value)
{
    return encodesimplevalue(ts, value, "%" PRIu64);
}

bool ThingSetTextEncoder::encode(uint64_t *value)
{
    return encodesimplevalue(ts, value, "%" PRIu64);
}

bool ThingSetTextEncoder::encode(const int8_t &value)
{
    return encodesimplevalue(ts, value, "%" PRIi8);
}

bool ThingSetTextEncoder::encode(int8_t &value)
{
    return encodesimplevalue(ts, value, "%" PRIi8);
}

bool ThingSetTextEncoder::encode(const int16_t &value)
{
    return encodesimplevalue(ts, value, "%" PRIi16);
}

bool ThingSetTextEncoder::encode(int16_t &value)
{
    return encodesimplevalue(ts, value, "%" PRIi16);
}

bool ThingSetTextEncoder::encode(int16_t *value)
{
    return encodesimplevalue(ts, value, "%" PRIi16);
}

bool ThingSetTextEncoder::encode(const int32_t &value)
{
    return encodesimplevalue(ts, value, "%" PRIi32);
}

bool ThingSetTextEncoder::encode(int32_t &value)
{
    return encodesimplevalue(ts, value, "%" PRIi32);
}

bool ThingSetTextEncoder::encode(int32_t *value)
{
    return encodesimplevalue(ts, value, "%" PRIi32);
}

bool ThingSetTextEncoder::encode(const int64_t &value)
{
    return encodeSimpleValue(ts, value, "%" PRIi64);
}

bool ThingSetTextEncoder::encode(int64_t &value)
{
    return encodeSimpleValue(ts, value, "%" PRIi64);
}

bool ThingSetTextEncoder::encode(int64_t *value)
{
    return encodeSimpleValue(ts, value, "%" PRIi64);
}

// todo check this
bool ThingSetTextEncoder::encodeNull()
{
    return encodeSimpleValue(ts, NULL, "%s");
}

bool ThingSetTextEncoder::encodeMapStart(struct thingset_context *ts)
{
    return encodeSimpleValue(ts, '{', "%c");
}

bool ThingSetTextEncoder::encodeMapEnd(struct thingset_context *ts)
{
    return encodeSimpleValue(ts, '}', "%c");
}

bool ThingSetTextEncoder::encodeListStart(struct thingset_context *ts)
{
    return encodeSimpleValue(ts, '[', "%c");
}

bool ThingSetTextEncoder::encodeListEnd(struct thingset_context *ts)
{
    return encodeSimpleValue(ts, ']', "%c");
}

template <typename T> void encodeThingsetValue(std::string name, T value)
{
    encode(name);
    encode(":");
    encode(value);
    encode(",");
}

} // namespace ThingSet