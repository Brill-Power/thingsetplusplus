/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "thingset++/ThingSetDecoder.hpp"
#include "internal/bind_to_tuple.hpp"
#define JSMN_HEADER
#include "internal/jsmn.h"
#include "zcbor_decode.h"
#include <array>
#include <map>
#include <optional>
#include <tuple>

#define TEXT_ENCODER_BUFFER_SIZE 1024
#define TEXT_DECODER_DEFAULT_MAX_TOKENS 64

namespace ThingSet {

/// @brief Text protocol decoder for ThingSet.
class ThingSetTextDecoder : public ThingSetDecoder
{
protected:
    const char *_inputBuffer;
    const size_t _bufferSize;
    size_t _bufferElemPtr;
    size_t _tokenPtr;
    jsmn_parser _parser;

    ThingSetTextDecoder(const char *buffer, const size_t size);

    /// @brief Gets whether the stream that this decoder is decoding
    /// is forward-only.
    /// @return True if forward-only, otherwise false.
    virtual bool getIsForwardOnly() const;

    virtual jsmntok *getTokens() = 0;

public:
    using ThingSetDecoder::decode;
    bool decode(std::string *value) override;
    bool decode(char *value) override;
    bool decode(float *value) override;
    bool decode(double *value) override;
    bool decode(bool *value) override;
    bool decode(uint8_t *value) override;
    bool decode(uint16_t *value) override;
    bool decode(uint32_t *value) override;
    bool decode(uint64_t *value) override;
    bool decode(int8_t *value) override;
    bool decode(int16_t *value) override;
    bool decode(int32_t *value) override;
    bool decode(int64_t *value) override;
    bool decodeNull() override;
    bool decodeListStart() override;
    bool decodeListEnd() override;
    bool skip() override;

    ThingSetEncodedNodeType peekType() override;

protected:
    bool decodeMapStart() override;
    bool decodeMapEnd() override;
    bool isInMap() override;
    bool isInList() override;
    bool ensureListSize(const size_t size, size_t &elementCount) override;

private:
    bool expectType(const jsmntype_t &type, jsmntok **token);
    bool isLiteral(const char* literal);

    template <typename T> bool decodePrimitive(T *value, T (*parseFunc)(const char *, char **))
    {
        jsmntok *token;
        if (!expectType(JSMN_PRIMITIVE, &token)) {
            return false;
        }
        const char *startPtr = &_inputBuffer[token->start];
        char *endPtr;
        *value = parseFunc(startPtr, &endPtr);
        size_t consumed = endPtr - startPtr;
        _bufferElemPtr += consumed;
        return true;
    }
};

template <size_t tokens = TEXT_DECODER_DEFAULT_MAX_TOKENS>
class FixedSizeThingSetTextDecoder : public virtual ThingSetTextDecoder
{
private:
    std::array<jsmntok, tokens> _tokens;

protected:
    jsmntok *getTokens() override
    {
        if (_parser.toknext == 0) {
            jsmn_parse(&_parser, _inputBuffer, _bufferSize, _tokens.data(), tokens);
        }
        return _tokens.data();
    }

public:
    FixedSizeThingSetTextDecoder(const char *buffer, const size_t size) : ThingSetTextDecoder(buffer, size)
    {
    }

    template <size_t Size>
    FixedSizeThingSetTextDecoder(const std::array<char, Size> &buffer) : FixedSizeThingSetTextDecoder(buffer.data(), Size)
    {}
};

using DefaultFixedSizeThingSetTextDecoder = FixedSizeThingSetTextDecoder<TEXT_DECODER_DEFAULT_MAX_TOKENS>;

} // namespace ThingSet