/*
 * Copyright (c) 2024 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */
#include "ThingSetBinaryEncoder.hpp"

template <typename T, int size> bool ThingSetBinaryEncoder::encode(std::array<T, size> value)
{
    return this->encode(value.data(), value.size());
}

template <typename T, int size> bool ThingSetBinaryEncoder::encode(T value[size])
{
    return this->encode(value, size);
}

template <typename T> bool ThingSetBinaryEncoder::encode(T *value, size_t size)
{
    bool result = zcbor_list_start_encode(getEncoder(), size);
    for (size_t i = 0; i < size; i++) {
        result &= this->encode(value[i]);
    }
    return result && zcbor_list_end_encode(getEncoder(), size);
}

inline bool encode_and_shift(ThingSetBinaryEncoder *encoder)
{
    return true;
}

template <typename T, typename... TArgs>
bool encode_and_shift(ThingSetBinaryEncoder *encoder, T arg, TArgs... rest)
{
    return encoder->encode(arg) && encode_and_shift(encoder, rest...);
}

template <typename... TArgs> bool ThingSetBinaryEncoder::encode(TArgs... args)
{
    const size_t count = sizeof...(TArgs);
    return zcbor_list_start_encode(getEncoder(), count) && encode_and_shift(this, args...)
           && zcbor_list_end_encode(getEncoder(), count);
}

template <int size, int depth>
ThingSetBinaryEncoder ThingSetBinaryEncoder::create(uint8_t buffer[size])
{
    return FixedSizeThingSetBinaryEncoder<size, depth>(buffer);
}

template <int size, int depth>
zcbor_state_t *FixedSizeThingSetBinaryEncoder<size, depth>::getEncoder()
{
    return _encoder;
}

template <int size, int depth>
size_t FixedSizeThingSetBinaryEncoder<size, depth>::getEncodedLength()
{
    return _encoder->payload - _buffer;
}

template <int size, int depth>
FixedSizeThingSetBinaryEncoder<size, depth>::FixedSizeThingSetBinaryEncoder(uint8_t buffer[size])
    : _buffer(buffer)
{
    zcbor_new_encode_state(_encoder, depth, buffer, size, 2);
}
