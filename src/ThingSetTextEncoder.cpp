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
    bool ret = true;
    ret |= addResponseValue("\"", "%s");
    ret |= addResponseValue(value, "%.*s");
    ret |= addResponseValue("\"", "%s");
    return ret;
}

bool ThingSetTextEncoder::encode(std::string_view &value)
{
    bool ret = true;
    ret |= addResponseValue("\"", "%s");
    ret |= addResponseValue(value, "%.*s");
    ret |= addResponseValue("\"", "%s");
    return ret;
}

bool ThingSetTextEncoder::encode(const std::string &value)
{
    bool ret = true;
    ret |= addResponseValue("\"", "%s");
    ret |= addResponseValue(value.c_str(), "%s");
    ret |= addResponseValue("\"", "%s");
    return ret;
}

bool ThingSetTextEncoder::encode(std::string &value)
{
    bool ret = true;
    ret |= addResponseValue("\"", "%s");
    ret |= addResponseValue(value.c_str(), "%s");
    ret |= addResponseValue("\"", "%s");
    return ret;
}

bool ThingSetTextEncoder::encode(const char *value)
{
    bool ret = true;
    ret |= addResponseValue("\"", "%s");
    ret |= addResponseValue(value, "%s");
    ret |= addResponseValue("\"", "%s");
    return ret;
}

bool ThingSetTextEncoder::encode(char *value)
{
    bool ret = true;
    ret |= addResponseValue("\"", "%s");
    ret |= addResponseValue(value, "%s");
    ret |= addResponseValue("\"", "%s");
    return ret;
}

bool ThingSetTextEncoder::encode(const float &value)
{
    return addResponseValue((double)value, "%f");
}

bool ThingSetTextEncoder::encode(float &value)
{
    return addResponseValue((double)value, "%f");
}

bool ThingSetTextEncoder::encode(float *value)
{
    return addResponseValue(*value, "%f");
}

bool ThingSetTextEncoder::encode(const double &value)
{
    return addResponseValue(value, "%f");
}

bool ThingSetTextEncoder::encode(double &value)
{
    return addResponseValue(value, "%f");
}

bool ThingSetTextEncoder::encode(double *value)
{
    return addResponseValue(*value, "%f");
}

// todo bool may not be needed as handled by int (1/0 instead of true/false)
// bool ThingSetTextEncoder::encode(const bool &value)
// {
//     return zcbor_bool_put(getState(), value);
// }

// bool ThingSetTextEncoder::encode(bool &value)
// {
//     return zcbor_bool_put(getState(), value);
// }

bool ThingSetTextEncoder::encode(const uint8_t &value)
{
    return addResponseValue(value, "%" PRIu8);
}

bool ThingSetTextEncoder::encode(uint8_t &value)
{
    return addResponseValue(value, "%" PRIu8);
}

bool ThingSetTextEncoder::encode(uint8_t *value)
{
    return addResponseValue(*value, "%" PRIu8);
}

bool ThingSetTextEncoder::encode(const uint16_t &value)
{
    return addResponseValue(value, "%" PRIu16);
}

bool ThingSetTextEncoder::encode(uint16_t &value)
{
    return addResponseValue(value, "%" PRIu16);
}

bool ThingSetTextEncoder::encode(uint16_t *value)
{
    return addResponseValue(*value, "%" PRIu16);
}

bool ThingSetTextEncoder::encode(const uint32_t &value)
{
    return addResponseValue(value, "%" PRIu32);
}

bool ThingSetTextEncoder::encode(uint32_t &value)
{
    return addResponseValue(value, "%" PRIu32);
}

bool ThingSetTextEncoder::encode(uint32_t *value)
{
    return addResponseValue(*value, "%" PRIu32);
}

bool ThingSetTextEncoder::encode(const uint64_t &value)
{
    return addResponseValue(value, "%" PRIu64);
}

bool ThingSetTextEncoder::encode(uint64_t &value)
{
    return addResponseValue(value, "%" PRIu64);
}

bool ThingSetTextEncoder::encode(uint64_t *value)
{
    return addResponseValue(*value, "%" PRIu64);
}

bool ThingSetTextEncoder::encode(const int8_t &value)
{
    return addResponseValue(value, "%" PRIi8);
}

bool ThingSetTextEncoder::encode(int8_t &value)
{
    return addResponseValue(value, "%" PRIi8);
}

bool ThingSetTextEncoder::encode(const int16_t &value)
{
    return addResponseValue(value, "%" PRIi16);
}

bool ThingSetTextEncoder::encode(int16_t &value)
{
    return addResponseValue(value, "%" PRIi16);
}

bool ThingSetTextEncoder::encode(int16_t *value)
{
    return addResponseValue(*value, "%" PRIi16);
}

bool ThingSetTextEncoder::encode(const int32_t &value)
{
    return addResponseValue(value, "%" PRIi32);
}

bool ThingSetTextEncoder::encode(int32_t &value)
{
    return addResponseValue(value, "%" PRIi32);
}

bool ThingSetTextEncoder::encode(int32_t *value)
{
    return addResponseValue(*value, "%" PRIi32);
}

bool ThingSetTextEncoder::encode(const int64_t &value)
{
    return addResponseValue(value, "%" PRIi64);
}

bool ThingSetTextEncoder::encode(int64_t &value)
{
    return addResponseValue(value, "%" PRIi64);
}

bool ThingSetTextEncoder::encode(int64_t *value)
{
    return addResponseValue(*value, "%" PRIi64);
}

// todo check this
bool ThingSetTextEncoder::encodeNull()
{
    return addResponseValue(NULL, "%s");
}

bool ThingSetTextEncoder::encodeMapStart()
{
    _depth++;
    return addResponseValue("{", "%s");
}

bool ThingSetTextEncoder::encodeMapStart(uint32_t)
{
    return encodeMapStart();
}

bool ThingSetTextEncoder::encodeMapEnd()
{
    _depth--;
    _rsp_pos--;
    bool ret = addResponseValue("}", "%s");
    if (_depth != 0) {
        ret |= addResponseValue(",", "%s");
    }
    return ret;
}

bool ThingSetTextEncoder::encodeMapEnd(uint32_t)
{
    return encodeMapEnd();
}

bool ThingSetTextEncoder::encodeListStart()
{
    _depth++;
    return addResponseValue("[", "%s");
}

bool ThingSetTextEncoder::encodeListStart(uint32_t)
{
    return encodeListStart();
}

bool ThingSetTextEncoder::encodeListEnd()
{
    _depth--;
    _rsp_pos--;
    bool ret = addResponseValue("]", "%s");
    if (_depth != 0) {
        ret |= addResponseValue(",", "%s");
    }
    return ret;
}

bool ThingSetTextEncoder::encodeListEnd(uint32_t)
{
    return encodeListEnd();
}

// template <typename T> void encodeThingsetValue(std::string name, T value)
// {
//     encode(name);
//     encode(":");
//     encode(value);
//     encode(",");
// }

} // namespace ThingSet