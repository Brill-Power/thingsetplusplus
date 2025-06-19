/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "thingset++/ThingSetDecoder.hpp"
#include "internal/bind_to_tuple.hpp"
#include "zcbor_decode.h"
#include <array>
#include <functional>
#include <map>
#include <optional>
#include <string>
#include <tuple>

#define BINARY_DECODER_MAX_NULL_TERMINATED_STRING_LENGTH 255
#define BINARY_DECODER_DEFAULT_MAX_DEPTH                 8

namespace ThingSet {

class ThingSetBinaryDecoder;

/// @brief Options to control the behaviour of the decoder.
enum ThingSetBinaryDecoderOptions
{
    /// @brief If set, permits the decoding of arrays which are smaller than the declared
    /// size of the destination array.
    allowUndersizedArrays = 1 << 0,
};

/// @brief Interface for values that can be decoded with a binary encoder.
class ThingSetBinaryDecodable
{
public:
    virtual bool decode(ThingSetBinaryDecoder &decoder) = 0;
};

/// @brief Binary protocol decoder for ThingSet.
class ThingSetBinaryDecoder : public ThingSetDecoder
{
private:
    ThingSetBinaryDecoderOptions _options;

protected:
    ThingSetBinaryDecoder();
    ThingSetBinaryDecoder(ThingSetBinaryDecoderOptions options);

    static void initialiseState(zcbor_state_t *state, const size_t depth, const uint8_t *buffer, const size_t size, const int elementCount);

    virtual zcbor_state_t *getState() = 0;

    /// @brief Gets whether the stream that this decoder is decoding
    /// is forward-only.
    /// @return True if forward-only, otherwise false.
    virtual bool getIsForwardOnly() const;

public:
    virtual size_t getDecodedLength() = 0;

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

    zcbor_major_type_t peekType();
    bool skip() override;
    bool skipUntil(zcbor_major_type_t sought);

protected:
    bool decodeMapStart() override;
    bool decodeMapEnd() override;
    bool isInMap() override;
    bool isInList() override;
    bool ensureListSize(const size_t size, size_t &elementCount) override;
};

template <int depth = BINARY_DECODER_DEFAULT_MAX_DEPTH>
class FixedDepthThingSetBinaryDecoder : public virtual ThingSetBinaryDecoder
{
private:
    // The start of the buffer
    const uint8_t *_buffer;
    std::array<zcbor_state_t, depth> _state;

protected:
    zcbor_state_t *getState() override
    {
        return _state.data();
    }

public:
    FixedDepthThingSetBinaryDecoder(const uint8_t *buffer, const size_t size, ThingSetBinaryDecoderOptions options)
        : FixedDepthThingSetBinaryDecoder(buffer, size, 1, options)
    {}

    template <size_t Size>
    FixedDepthThingSetBinaryDecoder(const std::array<uint8_t, Size> &buffer)
        : FixedDepthThingSetBinaryDecoder(buffer.data(), Size)
    {}

    FixedDepthThingSetBinaryDecoder(const uint8_t *buffer, const size_t size)
        : FixedDepthThingSetBinaryDecoder(buffer, size, ThingSetBinaryDecoderOptions{})
    {}

    template <size_t Size>
    FixedDepthThingSetBinaryDecoder(const std::array<uint8_t, Size> &buffer, int elementCount)
        : FixedDepthThingSetBinaryDecoder(buffer.data(), Size, elementCount)
    {}

    FixedDepthThingSetBinaryDecoder(const uint8_t *buffer, const size_t size, int elementCount)
        : FixedDepthThingSetBinaryDecoder(buffer, size, elementCount, ThingSetBinaryDecoderOptions{})
    {}

    FixedDepthThingSetBinaryDecoder(const uint8_t *buffer, const size_t size, int elementCount,
                                    ThingSetBinaryDecoderOptions options)
        : ThingSetBinaryDecoder(options), _buffer(buffer)
    {
        initialiseState(_state.data(), depth, buffer, size, elementCount);
    }

    size_t getDecodedLength() override
    {
        return _state.data()->payload - _buffer;
    }
};

using DefaultFixedDepthThingSetBinaryDecoder = FixedDepthThingSetBinaryDecoder<BINARY_DECODER_DEFAULT_MAX_DEPTH>;

} // namespace ThingSet