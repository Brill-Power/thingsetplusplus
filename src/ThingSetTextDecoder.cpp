/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "thingset++/internal/jsmn.h"
#include "thingset++/ThingSetTextDecoder.hpp"
#include <cctype>

namespace ThingSet {

ThingSetTextDecoder::ThingSetTextDecoder(const char *buffer, const size_t size) : _inputBuffer(buffer), _bufferSize(size), _bufferElemPtr(0), _tokenIndex(0)
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
    *value = std::string(&_inputBuffer[token->start], token->end - token->start);
    // a string token excludes its quotes, so step past the closing one
    _bufferElemPtr = token->end + 1;
    return true;
}

bool ThingSetTextDecoder::decode(char *value, size_t size)
{
    jsmntok *token;
    if (!expectType(JSMN_STRING, &token)) {
        return false;
    }
    size_t length = (size_t)token->end - (size_t)token->start;
    // size is that of the buffer, which must also hold the terminator
    if (length < size) {
        strncpy(value, &_inputBuffer[token->start], length);
        value[length] = '\0';
        _bufferElemPtr = token->end + 1;
        return true;
    }
    return false;
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
    } else if (isLiteral("false")) {
        *value = false;
    } else {
        return false;
    }
    _bufferElemPtr = token->end;
    return true;
}

bool ThingSetTextDecoder::decode(uint8_t *value)
{
    return decodeInteger(value);
}

bool ThingSetTextDecoder::decode(uint16_t *value)
{
    return decodeInteger(value);
}

bool ThingSetTextDecoder::decode(uint32_t *value)
{
    return decodeInteger(value);
}

bool ThingSetTextDecoder::decode(uint64_t *value)
{
    return decodeInteger(value);
}

bool ThingSetTextDecoder::decode(int8_t *value)
{
    return decodeInteger(value);
}

bool ThingSetTextDecoder::decode(int16_t *value)
{
    return decodeInteger(value);
}

bool ThingSetTextDecoder::decode(int32_t *value)
{
    return decodeInteger(value);
}

bool ThingSetTextDecoder::decode(int64_t *value)
{
    return decodeInteger(value);
}

size_t ThingSetTextDecoder::nextNonWhitespace() const
{
    size_t pos = _bufferElemPtr;
    while (pos < _bufferSize && isspace((unsigned char)_inputBuffer[pos])) {
        pos++;
    }
    return pos;
}

bool ThingSetTextDecoder::isLiteral(const char* literal)
{
    return strncmp(&_inputBuffer[_bufferElemPtr], literal, strlen(literal)) == 0;
}

bool ThingSetTextDecoder::decodeNull()
{
    jsmntok *token;
    if (!expectType(JSMN_PRIMITIVE, &token) || !isLiteral("null")) {
        return false;
    }
    _bufferElemPtr = token->end;
    return true;
}

bool ThingSetTextDecoder::decodeListStart()
{
    return expectType(JSMN_ARRAY, nullptr);
}

bool ThingSetTextDecoder::decodeListEnd()
{
    // TODO: concoct better test that does not rely on exact formatting
    size_t pos = nextNonWhitespace();
    if (pos < _bufferSize && _inputBuffer[pos] == ']') {
        _bufferElemPtr = pos + 1;
        return true;
    }
    return false;
}

bool ThingSetTextDecoder::decodeBytes(uint8_t *, size_t, size_t &)
{
    // TODO: I guess this would be a base64-encoded string or something
    return false;
}

bool ThingSetTextDecoder::decodeMapStart()
{
    return expectType(JSMN_OBJECT, nullptr);
}

bool ThingSetTextDecoder::decodeMapEnd()
{
    // TODO: concoct better test that does not rely on exact formatting
    size_t pos = nextNonWhitespace();
    if (pos < _bufferSize && _inputBuffer[pos] == '}') {
        _bufferElemPtr = pos + 1;
        return true;
    }
    return false;
}

bool ThingSetTextDecoder::isInMap() const
{
    // TODO: concoct better test that does not rely on exact formatting
    size_t pos = nextNonWhitespace();
    return (pos < _bufferSize) && (_inputBuffer[pos] != '}');
}

bool ThingSetTextDecoder::isInList() const
{
    // TODO: concoct better test that does not rely on exact formatting
    size_t pos = nextNonWhitespace();
    return (pos < _bufferSize) && (_inputBuffer[pos] != ']');
}

ThingSetEncodedNodeType ThingSetTextDecoder::peekType()
{
    jsmntok *token = &getTokens()[_tokenIndex];
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
    jsmntok *token = &getTokens()[_tokenIndex];
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
    _tokenIndex++;
    return true;
}

bool ThingSetTextDecoder::skip()
{
    _tokenIndex++;
    jsmntok *token = &getTokens()[_tokenIndex];
    _bufferElemPtr = token->start;
    return true;
}

} // namespace ThingSet