/*
 * Copyright (c) 2024 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */
#pragma once

#include "zcbor_encode.h"
#include <array>

#define BINARY_ENCODER_MAX_NULL_TERMINATED_STRING_LENGTH 255
#define BINARY_ENCODER_DEFAULT_MAX_DEPTH                 6

namespace ThingSet {

/// @brief Binary protocol encoder for ThingSet.
class ThingSetBinaryEncoder
{
protected:
    virtual zcbor_state_t *getEncoder();

public:
    template <typename T, size_t size> bool encode(std::array<T, size> &value);
    template <typename T, size_t size> bool encode(T value[size]);
    bool encode(char *&value);
    bool encode(float &value);
    bool encode(double &value);
    bool encode(bool &value);
    bool encode(uint8_t &value);
    bool encode(uint16_t &value);
    bool encode(uint32_t &value);
    bool encode(uint64_t &value);
    bool encode(int8_t &value);
    bool encode(int16_t &value);
    bool encode(int32_t &value);
    bool encode(int64_t &value);
    template <typename... TArgs> bool encode(TArgs... args);

    virtual size_t getEncodedLength();

private:
    template <typename T> bool encode(T *value, size_t size);
};

template <int size, int depth = BINARY_ENCODER_DEFAULT_MAX_DEPTH>
class FixedSizeThingSetBinaryEncoder : public ThingSetBinaryEncoder
{
    friend class ThingSetBinaryEncoder;

private:
    // The start of the buffer
    const uint8_t *_buffer;
    zcbor_state_t _encoder[depth];

protected:
    zcbor_state_t *getEncoder() override;

public:
    FixedSizeThingSetBinaryEncoder(uint8_t buffer[size]);
    size_t getEncodedLength() override;
};

/// @brief Interface for values that can be encoded with a binary encoder.
class ThingSetBinaryEncodable
{
    virtual bool encode(ThingSetBinaryEncoder &encoder) = 0;
};

}; // namespace ThingSet

#include "ThingSetBinaryEncoder.tpp"
