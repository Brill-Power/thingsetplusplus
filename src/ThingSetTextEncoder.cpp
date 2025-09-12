/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "thingset++/ThingSetTextEncoder.hpp"
#include <inttypes.h>

namespace ThingSet {

bool ThingSetTextEncoder::encode(const std::string_view &value)
{
    return encode(value.data());
}

bool ThingSetTextEncoder::encode(std::string_view &value)
{
    return encode(value.data());
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
    return append('"') && appendFormat(value, "%s") && append('"');
}

bool ThingSetTextEncoder::encode(char *value)
{
    return encode(static_cast<const char *>(value));
}

bool ThingSetTextEncoder::encode(const float &value)
{
    return appendFormat((double)value, "%f");
}

bool ThingSetTextEncoder::encode(float &value)
{
    return appendFormat((double)value, "%f");
}

bool ThingSetTextEncoder::encode(float *value)
{
    return appendFormat((double)*value, "%f");
}

bool ThingSetTextEncoder::encode(const double &value)
{
    return appendFormat(value, "%f");
}

bool ThingSetTextEncoder::encode(double &value)
{
    return appendFormat(value, "%f");
}

bool ThingSetTextEncoder::encode(double *value)
{
    return appendFormat(*value, "%f");
}

bool ThingSetTextEncoder::encode(const bool &value)
{
    return appendFormat(value, "%i");
}

bool ThingSetTextEncoder::encode(bool &value)
{
    return appendFormat(value, "%i");
}

bool ThingSetTextEncoder::encode(bool *value)
{
    return appendFormat(*value, "%i");
}

bool ThingSetTextEncoder::encode(const uint8_t &value)
{
    return appendFormat(value, "%" PRIu8);
}

bool ThingSetTextEncoder::encode(uint8_t &value)
{
    return appendFormat(value, "%" PRIu8);
}

bool ThingSetTextEncoder::encode(uint8_t *value)
{
    return appendFormat(*value, "%" PRIu8);
}

bool ThingSetTextEncoder::encode(const uint16_t &value)
{
    return appendFormat(value, "%" PRIu16);
}

bool ThingSetTextEncoder::encode(uint16_t &value)
{
    return appendFormat(value, "%" PRIu16);
}

bool ThingSetTextEncoder::encode(uint16_t *value)
{
    return appendFormat(*value, "%" PRIu16);
}

bool ThingSetTextEncoder::encode(const uint32_t &value)
{
    return appendFormat(value, "%" PRIu32);
}

bool ThingSetTextEncoder::encode(uint32_t &value)
{
    return appendFormat(value, "%" PRIu32);
}

bool ThingSetTextEncoder::encode(uint32_t *value)
{
    return appendFormat(*value, "%" PRIu32);
}

bool ThingSetTextEncoder::encode(const uint64_t &value)
{
    return appendFormat(value, "%" PRIu64);
}

bool ThingSetTextEncoder::encode(uint64_t &value)
{
    return appendFormat(value, "%" PRIu64);
}

bool ThingSetTextEncoder::encode(uint64_t *value)
{
    return appendFormat(*value, "%" PRIu64);
}

bool ThingSetTextEncoder::encode(const int8_t &value)
{
    return appendFormat(value, "%" PRIi8);
}

bool ThingSetTextEncoder::encode(int8_t &value)
{
    return appendFormat(value, "%" PRIi8);
}

bool ThingSetTextEncoder::encode(int8_t *value)
{
    return appendFormat(*value, "%" PRIi8);
}

bool ThingSetTextEncoder::encode(const int16_t &value)
{
    return appendFormat(value, "%" PRIi16);
}

bool ThingSetTextEncoder::encode(int16_t &value)
{
    return appendFormat(value, "%" PRIi16);
}

bool ThingSetTextEncoder::encode(int16_t *value)
{
    return appendFormat(*value, "%" PRIi16);
}

bool ThingSetTextEncoder::encode(const int32_t &value)
{
    return appendFormat(value, "%" PRIi32);
}

bool ThingSetTextEncoder::encode(int32_t &value)
{
    return appendFormat(value, "%" PRIi32);
}

bool ThingSetTextEncoder::encode(int32_t *value)
{
    return appendFormat(*value, "%" PRIi32);
}

bool ThingSetTextEncoder::encode(const int64_t &value)
{
    return appendFormat(value, "%" PRIi64);
}

bool ThingSetTextEncoder::encode(int64_t &value)
{
    return appendFormat(value, "%" PRIi64);
}

bool ThingSetTextEncoder::encode(int64_t *value)
{
    return appendFormat(*value, "%" PRIi64);
}

bool ThingSetTextEncoder::encodeNull()
{
    return appendFormat("null", "%s");
}

bool ThingSetTextEncoder::encodePreamble()
{
    return true;
}

bool ThingSetTextEncoder::encodeMapStart()
{
    _depth++;
    return append('{');
}

bool ThingSetTextEncoder::encodeMapStart(const uint32_t &)
{
    return encodeMapStart();
}

bool ThingSetTextEncoder::encodeMapEnd()
{
    _depth--;
    _responsePosition--;
    return append('}');
}

bool ThingSetTextEncoder::encodeMapEnd(const uint32_t &)
{
    return encodeMapEnd();
}

bool ThingSetTextEncoder::encodeListStart()
{
    _depth++;
    return append('[');
}

bool ThingSetTextEncoder::encodeListStart(const uint32_t &)
{
    return encodeListStart();
}

bool ThingSetTextEncoder::encodeListEnd()
{
    _depth--;
    _responsePosition--;
    return append(']');
}

bool ThingSetTextEncoder::encodeListEnd(const uint32_t &)
{
    return encodeListEnd();
}

bool ThingSetTextEncoder::encodeBytes(const uint8_t *, const size_t &)
{
    return false;
}

bool ThingSetTextEncoder::encodeListSeparator()
{
    return append(',');
}

bool ThingSetTextEncoder::encodeKeyValuePairSeparator()
{
    return append(':');
}

bool ThingSetTextEncoder::encodeKeysAsIds() const
{
    return false;
}

} // namespace ThingSet