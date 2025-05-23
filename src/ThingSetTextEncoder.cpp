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
template <typename T> inline bool ThingSetTextEncoder::encodeSimpleValue(T value, const char *format)
{

    // todo work out how to reuse this properly, move to getstate function? this will only encode one value at a time
    // currently

    // todo move this to a getFutureEncodedLength in header file?
    int bytes_required = snprintf(nullptr, 0, format, value); // determine the length of value as a string

    if (sizeof(this->_rsp) < this->_rsp_pos + bytes_required) {
        return false;
    }

    // todo use getEncodedLength instead of sizeof section here?
    int bytes_written = snprintf(this->_rsp + this->_rsp_pos, sizeof(this->_rsp) - this->_rsp_pos, format, value);
    this->_rsp_pos += bytes_written;

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
    return encodeSimpleValue(value, "%.*s");
}

bool ThingSetTextEncoder::encode(std::string_view &value)
{
    return encodeSimpleValue(value, "%.*s");
}

bool ThingSetTextEncoder::encode(const std::string &value)
{
    return encodeSimpleValue(value.c_str(), "%s");
}

bool ThingSetTextEncoder::encode(std::string &value)
{
    return encodeSimpleValue(value.c_str(), "%s");
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

// todo check this
bool ThingSetTextEncoder::encodeNull()
{
    return encodeSimpleValue(NULL, "%s");
}

bool ThingSetTextEncoder::encodeMapStart()
{
    return encodeSimpleValue('{', "%c");
}

bool ThingSetTextEncoder::encodeMapStart(uint32_t count)
{
    return encodeMapStart();
}

bool ThingSetTextEncoder::encodeMapEnd()
{
    if (this->_rsp[this->_rsp_pos--] == ',') {
        this->_rsp_pos--;
    }
    bool ret = encodeSimpleValue('}', "%c");
    ret |= encodeSimpleValue(',', "%c");
    return ret;
}

bool ThingSetTextEncoder::encodeMapEnd(uint32_t count)
{
    return encodeMapEnd();
}

bool ThingSetTextEncoder::encodeListStart()
{
    return encodeSimpleValue('[', "%c");
}

bool ThingSetTextEncoder::encodeListStart(uint32_t count)
{
    return encodeListStart();
}

bool ThingSetTextEncoder::encodeListEnd()
{
    if (this->_rsp[this->_rsp_pos--] == ',') {
        this->_rsp_pos--;
    }
    bool ret = encodeSimpleValue(']', "%c");
    ret |= encodeSimpleValue(',', "%c");
    return ret;
}

bool ThingSetTextEncoder::encodeListEnd(uint32_t count)
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