/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "thingset++/internal/jsmn.h"
#include "thingset++/ThingSetTextDecoder.hpp"

namespace ThingSet {

ThingSetTextDecoder::ThingSetTextDecoder(const char *buffer, const size_t size) : _inputBuffer(buffer), _bufferSize(size), _bufferElemPtr(0), _tokenPtr(0)
{
    jsmn_init(&_parser);
}

bool ThingSetTextDecoder::getIsForwardOnly() const
{
    return false;
}

bool ThingSetTextDecoder::decode(std::string *value)
{
    jsmntok *token;
    if (!expectType(JSMN_STRING, &token)) {
        return false;
    }
    *value = std::string(&_inputBuffer[_bufferElemPtr], token->end - token->start);
    return true;
}

bool ThingSetTextDecoder::decode(char *value)
{
    jsmntok *token;
    if (!expectType(JSMN_STRING, &token)) {
        return false;
    }
    strncpy(value, &_inputBuffer[_bufferElemPtr], (size_t)token->end - (size_t)token->start);
    return true;
}

bool ThingSetTextDecoder::decode(float *value)
{
    auto decodeFunction = [](const char *start, char **end) -> float {
        return static_cast<float>(std::strtod(start, end));
    };
    return decodePrimitive<float>(value, decodeFunction);
}

bool ThingSetTextDecoder::decode(double *value)
{
    return decodePrimitive(value, std::strtod);
}

bool ThingSetTextDecoder::decode(bool *value)
{
    jsmntok *token;
    if (!expectType(JSMN_PRIMITIVE, &token)) {
        return false;
    }

    if (isLiteral("true")) {
        *value = true;
        return true;
    } else if (isLiteral("false")) {
        *value = false;
        return true;
    }
    return false;
}

bool ThingSetTextDecoder::decode(uint8_t *value)
{
    auto decodeFunction = [](const char *start, char **end) -> uint8_t {
        return static_cast<uint8_t>(std::strtoul(start, end, 10));
    };
    return decodePrimitive<uint8_t>(value, decodeFunction);
}

bool ThingSetTextDecoder::decode(uint16_t *value)
{
    auto decodeFunction = [](const char *start, char **end) -> uint16_t {
        return static_cast<uint16_t>(std::strtoul(start, end, 10));
    };
    return decodePrimitive<uint16_t>(value, decodeFunction);
}

bool ThingSetTextDecoder::decode(uint32_t *value)
{
    auto decodeFunction = [](const char *start, char **end) -> uint32_t {
        return static_cast<uint32_t>(std::strtoul(start, end, 10));
    };
    return decodePrimitive<uint32_t>(value, decodeFunction);
}

bool ThingSetTextDecoder::decode(uint64_t *value)
{
    auto decodeFunction = [](const char *start, char **end) -> uint64_t {
        return static_cast<uint64_t>(std::strtoull(start, end, 10));
    };
    return decodePrimitive<uint64_t>(value, decodeFunction);
}

bool ThingSetTextDecoder::decode(int8_t *value)
{
    auto decodeFunction = [](const char *start, char **end) -> int8_t {
        return static_cast<int8_t>(std::strtol(start, end, 10));
    };
    return decodePrimitive<int8_t>(value, decodeFunction);
}

bool ThingSetTextDecoder::decode(int16_t *value)
{
    auto decodeFunction = [](const char *start, char **end) -> int16_t {
        return static_cast<int16_t>(std::strtol(start, end, 10));
    };
    return decodePrimitive<int16_t>(value, decodeFunction);
}

bool ThingSetTextDecoder::decode(int32_t *value)
{
    auto decodeFunction = [](const char *start, char **end) -> int32_t {
        return static_cast<int32_t>(std::strtol(start, end, 10));
    };
    return decodePrimitive<int32_t>(value, decodeFunction);
}

bool ThingSetTextDecoder::decode(int64_t *value)
{
    auto decodeFunction = [](const char *start, char **end) -> int64_t {
        return static_cast<int64_t>(std::strtol(start, end, 10));
    };
    return decodePrimitive<int64_t>(value, decodeFunction);
}

bool ThingSetTextDecoder::isLiteral(const char* literal)
{
    return strncmp(&_inputBuffer[_bufferElemPtr], literal, strlen(literal)) == 0;
}

bool ThingSetTextDecoder::decodeNull()
{
    if (!expectType(JSMN_PRIMITIVE, nullptr)) {
        return false;
    }
    return isLiteral("null");
}

bool ThingSetTextDecoder::decodeListStart()
{
    return expectType(JSMN_ARRAY, nullptr);
}

bool ThingSetTextDecoder::decodeListEnd()
{
    // TODO: concoct better test that does not rely on exact formatting
    if (_inputBuffer[_bufferElemPtr] == ']') {
        _bufferElemPtr++;
        return true;
    }
    return false;
}

bool ThingSetTextDecoder::decodeMapStart()
{
    return expectType(JSMN_OBJECT, nullptr);
}

bool ThingSetTextDecoder::decodeMapEnd()
{
    // TODO: concoct better test that does not rely on exact formatting
    if (_inputBuffer[_bufferElemPtr] == '}') {
        _bufferElemPtr++;
        return true;
    }
    return false;
}

bool ThingSetTextDecoder::isInMap() const
{
    // TODO: concoct better test that does not rely on exact formatting
    return (_bufferElemPtr < _bufferSize) && (_inputBuffer[_bufferElemPtr] != '}');
}

bool ThingSetTextDecoder::isInList() const
{
    // TODO: concoct better test that does not rely on exact formatting
    return (_bufferElemPtr < _bufferSize) && (_inputBuffer[_bufferElemPtr] != ']');
}

ThingSetEncodedNodeType ThingSetTextDecoder::peekType()
{
    jsmntok *token = &getTokens()[_tokenPtr];
    switch (token->type)
    {
        case JSMN_PRIMITIVE:
            return ThingSetEncodedNodeType::primitive;
        case JSMN_STRING:
            return ThingSetEncodedNodeType::string;
        case JSMN_ARRAY:
            return ThingSetEncodedNodeType::list;
        case JSMN_OBJECT:
            return ThingSetEncodedNodeType::map;
        default:
            return ThingSetEncodedNodeType::unknown;
    }
}

bool ThingSetTextDecoder::ensureListSize(const size_t size, size_t &elementCount)
{
    elementCount = size;
    return true;
}

bool ThingSetTextDecoder::expectType(const jsmntype_t &type, jsmntok **t)
{
    jsmntok *token = &getTokens()[_tokenPtr];
    if (t) {
        *t = token;
    }
    if (token->type != type) {
        return false;
    }
    size_t offset = 0;
    if (token->type == JSMN_ARRAY || token->type == JSMN_OBJECT)
    {
        offset = 1;
    }
    _bufferElemPtr = token->start + offset;
    _tokenPtr++;
    return true;
}

bool ThingSetTextDecoder::skip()
{
    _tokenPtr++;
    jsmntok *token = &getTokens()[_tokenPtr];
    _bufferElemPtr = token->start;
    return true;
}

} // namespace ThingSet