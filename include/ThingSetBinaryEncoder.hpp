/*
 * Copyright (c) 2024 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */
#pragma once

#include "zcbor_encode.h"
#include <array>

/// @brief Binary protocol encoder for ThingSet.
class ThingSetBinaryEncoder
{
protected:
    virtual zcbor_state_t *getEncoder();

public:
    template <typename T, int size> bool encode(std::array<T, size> value);
    template <typename T, int size> bool encode(T value[size]);
    template <typename T> bool encode(T &value);
    template <typename... TArgs> bool encode(TArgs... args);

    virtual size_t getEncodedLength();

    template <int size, int depth> static ThingSetBinaryEncoder create(uint8_t buffer[size]);

private:
    template <typename T> bool encode(T *value, size_t size);
};

template <int size, int depth> class FixedSizeThingSetBinaryEncoder : public ThingSetBinaryEncoder
{
    friend class ThingSetBinaryEncoder;

private:
    // The start of the buffer
    uint8_t *_buffer;
    zcbor_state_t _encoder[depth];

    FixedSizeThingSetBinaryEncoder(uint8_t *buffer);

public:
    size_t getEncodedLength() override;
    zcbor_state_t *getEncoder() override;
};

/// @brief Interface for values that can be encoded with a binary encoder.
class ThingSetBinaryEncodable
{
    virtual bool encode(ThingSetBinaryEncoder &encoder) = 0;
};

#include "ThingSetBinaryEncoder.tpp"
