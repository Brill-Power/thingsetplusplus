/*
 * Copyright (c) 2025 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */

#include "thingset++/ThingSetTextEncoder.hpp"
#include "inttypes.h"

namespace ThingSet {

static bool encodeListMapEnd(zcbor_state_t *state);

inline bool encodeStart(struct thingset_context *ts, char c)
{
    if (ts->rsp_size > ts->rsp_pos + 2) {
        ts->rsp[ts->rsp_pos++] = c;
        return true;
    }
    else {
        ts->rsp_pos = 0;
        return false;
    }
}

inline bool encodeEnd(struct thingset_context *ts, char c)
{
    if (ts->rsp_size > ts->rsp_pos + 3) {
        if (ts->rsp[ts->rsp_pos - 1] == ',') {
            ts->rsp_pos--;
        }
        ts->rsp[ts->rsp_pos++] = c;
        ts->rsp[ts->rsp_pos++] = ',';
        return true;
    }
    else {
        ts->rsp_pos = 0;
        return false;
    }
}

// todo delete this comment
// encode(jsonEncoderState, T Property) returns true/flase, buf[1] = "a", buf[2] = "d", etc... cur_pos = 14

template <typename T>
bool encode(struct thingset_context *ts, ThingSetProperty<Id, ParentId, Name, Access, T> &property)
{
    returns true / flase, buf[1] = "a", buf[2] = "d",
                          etc... cur_pos = 14

                          if (ts->rsp_size > ts->rsp_pos + 2)
    {
        ts->rsp[ts->rsp_pos++] = c;
        return true;
    }
    else {
        ts->rsp_pos = 0;
        return false;
    }
}

// encodes a value of a simple type using a format specifier (eg %i, %f etc), convert this to work in this application
template <typename T> inline bool encodeSimpleValue(struct thingset_context *ts, T value, const char *format)
{
    int bytes_required = snprintf(nullptr, 0, format, value); // determine the length of value as a string

    if (sizeof(ts->rsp) < ts->rsp_pos + bytes_required) {
        return false;
    }

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

// todo not sure what to do here
// bool ThingSetTextEncoder::encode(const std::string_view &value)
// {
//     return encode(std::string(value));
// }

// bool ThingSetTextEncoder::encode(std::string_view &value)
// {
//     return encode(std::string(value));
// }

bool ThingSetTextEncoder::encode(const std::string &value)
{
    return encodeSimpleValue(value, "%s");
}

bool ThingSetTextEncoder::encode(std::string &value)
{
    return encodeSimpleValue(value, "%s");
}

bool ThingSetTextEncoder::encode(const char *value)
{
    return encodeSimpleValue(value, "%s");
}

bool ThingSetTextEncoder::encode(char *value)
{
    return encodeSimpleValue(value, "%s");
}

bool ThingSetTextEncoder::encode(const float &value)
{
    return encodeSimpleValue(value, "%f");
}

bool ThingSetTextEncoder::encode(float &value)
{
    return encodeSimpleValue(value, "%f");
}

bool ThingSetTextEncoder::encode(float *value)
{
    return encodeSimpleValue(value, "%f");
}

bool ThingSetTextEncoder::encode(const double &value)
{
    return encodeSimpleValue(value, "%f");
}

bool ThingSetTextEncoder::encode(double &value)
{
    return encodeSimpleValue(value, "%f");
}

bool ThingSetTextEncoder::encode(double *value)
{
    return encodeSimpleValue(value, "%f");
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
    return encodeSimpleValue(value, "%" PRIu8);
}

bool ThingSetTextEncoder::encode(uint8_t &value)
{
    return encodeSimpleValue(value, "%" PRIu8);
}

bool ThingSetTextEncoder::encode(uint8_t *value)
{
    return encodeSimpleValue(value, "%" PRIu8);
}

bool ThingSetTextEncoder::encode(const uint16_t &value)
{
    return encodeSimpleValue(value, "%" PRIu16);
}

bool ThingSetTextEncoder::encode(uint16_t &value)
{
    return encodeSimpleValue(value, "%" PRIu16);
}

bool ThingSetTextEncoder::encode(uint16_t *value)
{
    return encodeSimpleValue(value, "%" PRIu16);
}

bool ThingSetTextEncoder::encode(const uint32_t &value)
{
    return encodeSimpleValue(value, "%" PRIu32);
}

bool ThingSetTextEncoder::encode(uint32_t &value)
{
    return encodeSimpleValue(value, "%" PRIu32);
}

bool ThingSetTextEncoder::encode(uint32_t *value)
{
    return encodeSimpleValue(value, "%" PRIu32);
}

bool ThingSetTextEncoder::encode(const uint64_t &value)
{
    return encodeSimpleValue(value, "%" PRIu64);
}

bool ThingSetTextEncoder::encode(uint64_t &value)
{
    return encodeSimpleValue(value, "%" PRIu64);
}

bool ThingSetTextEncoder::encode(uint64_t *value)
{
    return encodeSimpleValue(value, "%" PRIu64);
}

bool ThingSetTextEncoder::encode(const int8_t &value)
{
    return encodeSimpleValue(value, "%" PRIi8);
}

bool ThingSetTextEncoder::encode(int8_t &value)
{
    return encodeSimpleValue(value, "%" PRIi8);
}

bool ThingSetTextEncoder::encode(const int16_t &value)
{
    return encodeSimpleValue(value, "%" PRIi16);
}

bool ThingSetTextEncoder::encode(int16_t &value)
{
    return encodeSimpleValue(value, "%" PRIi16);
}

bool ThingSetTextEncoder::encode(int16_t *value)
{
    return encodeSimpleValue(value, "%" PRIi16);
}

bool ThingSetTextEncoder::encode(const int32_t &value)
{
    return encodeSimpleValue(value, "%" PRIi32);
}

bool ThingSetTextEncoder::encode(int32_t &value)
{
    return encodeSimpleValue(value, "%" PRIi32);
}

bool ThingSetTextEncoder::encode(int32_t *value)
{
    return encodeSimpleValue(value, "%" PRIi32);
}

bool ThingSetTextEncoder::encode(const int64_t &value)
{
    return encodeSimpleValue(value, "%" PRIi64);
}

bool ThingSetTextEncoder::encode(int64_t &value)
{
    return encodeSimpleValue(value, "%" PRIi64);
}

bool ThingSetTextEncoder::encode(int64_t *value)
{
    return encodeSimpleValue(value, "%" PRIi64);
}

// todo not sure what to do here
// bool ThingSetTextEncoder::encodeNull()
// {
//     return zcbor_nil_put(this->getState(), NULL);
// }

bool ThingSetTextEncoder::encodeMapStart(struct thingset_context *ts)
{
    return encodeStart(ts, '{');
}

bool ThingSetTextEncoder::encodeMapEnd(struct thingset_context *ts)
{
    return encodeEnd(ts, '}');
}

bool ThingSetTextEncoder::encodeListStart(struct thingset_context *ts)
{
    return encodeStart(ts, '[');
}

bool ThingSetTextEncoder::encodeListEnd(struct thingset_context *ts)
{
    return encodeEnd(ts, ']');
}

} // namespace ThingSet