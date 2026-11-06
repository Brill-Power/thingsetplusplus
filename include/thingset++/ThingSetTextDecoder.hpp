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
#include <cerrno>
#include <cstdlib>
#include <limits>
#include <map>
#include <optional>
#include <tuple>
#include <type_traits>

#define TEXT_ENCODER_BUFFER_SIZE 1024
#define TEXT_DECODER_DEFAULT_MAX_TOKENS 64

namespace ThingSet {

/// @brief Text protocol decoder for ThingSet.
class ThingSetTextDecoder : public ThingSetDecoder
{
protected:
    const char *_inputBuffer;
    size_t _bufferSize;
    size_t _bufferElemPtr;
    size_t _tokenIndex;
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
    bool decode(char *value, size_t size) override;
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
    bool decodeBytes(uint8_t *buffer, size_t capacity, size_t &size) override;
    bool skip() override;

    ThingSetEncodedNodeType peekType() override;

protected:
    bool decodeMapStart() override;
    bool decodeMapEnd() override;
    bool isInMap() const override;
    bool isInList() const override;
    bool ensureListSize(const size_t size, size_t &elementCount) override;

private:
    bool expectType(const jsmntype_t &type, jsmntok **token);
    bool isLiteral(const char* literal);
    /// @brief Gets the position of the next character from the current one that is not whitespace.
    size_t nextNonWhitespace() const;

    // Each decode of a single value leaves _bufferElemPtr at the end of its token, so that the
    // end of an enclosing list or map can be found from there.

    template <typename T> bool decodePrimitive(T *value, T (*parseFunc)(const char *, char **))
    {
        jsmntok *token;
        if (!expectType(JSMN_PRIMITIVE, &token)) {
            return false;
        }
        const char *startPtr = &_inputBuffer[token->start];
        char *endPtr;
        T parsed = parseFunc(startPtr, &endPtr);
        if (endPtr == startPtr) {
            return false;
        }
        *value = parsed;
        _bufferElemPtr = token->end;
        return true;
    }

    /// @brief Decodes an integer, failing if the token is not a number or its value does not fit in T.
    template <typename T> bool decodeInteger(T *value)
    {
        jsmntok *token;
        if (!expectType(JSMN_PRIMITIVE, &token)) {
            return false;
        }
        const char *startPtr = &_inputBuffer[token->start];
        char *endPtr;
        bool fits;
        T parsed;
        errno = 0;
        if constexpr (std::is_signed_v<T>) {
            long long wide = std::strtoll(startPtr, &endPtr, 10);
            fits = wide >= std::numeric_limits<T>::min() && wide <= std::numeric_limits<T>::max();
            parsed = static_cast<T>(wide);
        }
        else {
            // strtoull accepts a leading minus sign and negates the result, so reject it here
            unsigned long long wide = std::strtoull(startPtr, &endPtr, 10);
            fits = *startPtr != '-' && wide <= std::numeric_limits<T>::max();
            parsed = static_cast<T>(wide);
        }
        if (endPtr == startPtr || errno == ERANGE || !fits) {
            return false;
        }
        *value = parsed;
        _bufferElemPtr = token->end;
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