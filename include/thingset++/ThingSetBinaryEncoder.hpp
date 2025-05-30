/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "ThingSetEncoder.hpp"
#include "zcbor_encode.h"

#define BINARY_ENCODER_MAX_NULL_TERMINATED_STRING_LENGTH 256
#define BINARY_ENCODER_DEFAULT_MAX_DEPTH                 8

namespace ThingSet {

/// @brief Binary protocol encoder for ThingSet.
class ThingSetBinaryEncoder : public ThingSetEncoder
{
protected:
    virtual bool ensureState();
    virtual zcbor_state_t *getState() = 0;
    virtual bool getIsForwardOnly() const;

public:
    using ThingSetEncoder::encode;
    bool encode(const std::string_view &value) override;
    bool encode(std::string_view &value) override;
    bool encode(const std::string &value) override;
    bool encode(std::string &value) override;
    bool encode(const char *value) override;
    bool encode(char *value) override;
    bool encode(const float &value) override;
    bool encode(float &value) override;
    bool encode(float *value) override;
    bool encode(const double &value) override;
    bool encode(double &value) override;
    bool encode(double *value) override;
    bool encode(const bool &value) override;
    bool encode(bool &value) override;
    bool encode(const uint8_t &value) override;
    bool encode(uint8_t &value) override;
    bool encode(uint8_t *value) override;
    bool encode(const uint16_t &value) override;
    bool encode(uint16_t &value) override;
    bool encode(uint16_t *value) override;
    bool encode(const uint32_t &value) override;
    bool encode(uint32_t &value) override;
    bool encode(uint32_t *value) override;
    bool encode(const uint64_t &value) override;
    bool encode(uint64_t &value) override;
    bool encode(uint64_t *value) override;
    bool encode(const int8_t &value) override;
    bool encode(int8_t &value) override;
    bool encode(const int16_t &value) override;
    bool encode(int16_t &value) override;
    bool encode(const int32_t &value) override;
    bool encode(int32_t &value) override;
    bool encode(int32_t *value) override;
    bool encode(const int64_t &value) override;
    bool encode(int64_t &value) override;
    bool encode(int64_t *value) override;
    bool encodeNull() override;
    bool encodePreamble() override;
    /// @brief Encode the start of a list. In forward-only encoding scenarios, you should
    /// use the overload which allows the number of elements in the list to be specified in
    /// advance.
    /// @return True if encoding succeeded, otherwise false.
    bool encodeListStart() override;
    /// @brief Encode the start of a list, specifying the number of elements the list
    /// will contain.
    /// @param count The number of elements in the list.
    /// @return True if encoding succeeded, otherwise false.
    bool encodeListStart(uint32_t count) override;
    bool encodeListEnd() override;
    /// @brief Encode the end of a list, specifying the number of elements the list
    /// contained. It should match the number passed to @ref encodeListStart.
    /// @param count The number of elements in the list.
    /// @return True if encoding succeeded, otherwise false.
    bool encodeListEnd(uint32_t count) override;
    bool encodeMapStart() override;
    /// @brief Encode the start of a map, specifying the number of key-value pairs the map
    /// will contain.
    /// @param count The number of pairs in the map.
    /// @return True if encoding succeeded, otherwise false.
    bool encodeMapStart(uint32_t count) override;
    bool encodeMapEnd() override;
    /// @brief Encode the end of a map, specifying the number of key-value pairs the map
    /// contained. It should match the number passed to @ref encodeMapStart.
    /// @param count The number of pairs in the map.
    /// @return True if encoding succeeded, otherwise false.
    bool encodeMapEnd(uint32_t count) override;

protected:
    bool encodeListSeparator() override;
    bool encodeKeyValuePairSeparator() override;
};

template <int depth = BINARY_ENCODER_DEFAULT_MAX_DEPTH>
class FixedDepthThingSetBinaryEncoder : public virtual ThingSetBinaryEncoder
{
private:
    // The start of the buffer
    const uint8_t *_buffer;
    zcbor_state_t _state[depth];

protected:
    zcbor_state_t *getState() override
    {
        return _state;
    }

public:
    FixedDepthThingSetBinaryEncoder(uint8_t *buffer, size_t size) : FixedDepthThingSetBinaryEncoder(buffer, size, 1)
    {}

    FixedDepthThingSetBinaryEncoder(uint8_t *buffer, size_t size, size_t elementCount) : _buffer(buffer)
    {
        zcbor_new_encode_state(_state, depth, buffer, size, elementCount);
    }

    size_t getEncodedLength() const override
    {
        return _state->payload - _buffer;
    }
};

using DefaultFixedDepthThingSetBinaryEncoder = FixedDepthThingSetBinaryEncoder<BINARY_ENCODER_DEFAULT_MAX_DEPTH>;

}; // namespace ThingSet
