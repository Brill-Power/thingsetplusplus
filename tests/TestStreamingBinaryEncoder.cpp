/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "thingset++/StreamingThingSetBinaryEncoder.hpp"
#include "gtest/gtest.h"

using namespace ThingSet;

constexpr size_t MessageSize = 8;
constexpr size_t TotalSize = 64;

class InMemoryStreamingThingSetBinaryEncoder : public StreamingThingSetBinaryEncoder<MessageSize>
{
public:
    std::array<uint8_t, TotalSize> output;
    size_t position = 0;

    InMemoryStreamingThingSetBinaryEncoder() : StreamingThingSetBinaryEncoder()
    {
        zcbor_new_encode_state(_state, BINARY_ENCODER_DEFAULT_MAX_DEPTH, &_buffer[0], _buffer.size(), 2);
    }

protected:
    bool write(size_t length, bool flushing) override
    {
        if (length > MessageSize) {
            return false;
        }
        memcpy(&output[position], &_buffer[0], length);
        position += length;
        return true;
    }
};

TEST(StreamingBinaryEncoder, EncodeFloat)
{
    InMemoryStreamingThingSetBinaryEncoder encoder;
    encoder.encode(3.14f);
    encoder.flush();
    ASSERT_EQ(5, encoder.position);
}

TEST(StreamingBinaryEncoder, EncodeIntArray)
{
    InMemoryStreamingThingSetBinaryEncoder encoder;
    std::array<int, 4> array = { 1, 2, 3, 4 };
    encoder.encode(array);
    encoder.flush();
    ASSERT_EQ(5, encoder.position);
}

TEST(StreamingBinaryEncoder, EncodeOverflow)
{
    // rather contrived way of getting the encoder to not write before a flush
    InMemoryStreamingThingSetBinaryEncoder encoder;
    encoder.encode("Hello!");
    ASSERT_EQ(0, encoder.position);
    encoder.encode(32768);
    encoder.flush();
    ASSERT_EQ(10, encoder.position);
}