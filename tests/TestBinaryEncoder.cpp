/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "thingset++/ThingSetBinaryEncoder.hpp"
#include "gtest/gtest.h"

using namespace ThingSet;

#define SETUP(size)                                                                                                    \
    uint8_t buffer[size];                                                                                              \
    FixedDepthThingSetBinaryEncoder encoder(buffer, size);

#define ASSERT_BUFFER_EQ(expected, actual, actual_len)                                                                 \
    ASSERT_EQ(sizeof(expected), actual_len);                                                                           \
    for (size_t i = 0; i < sizeof(expected); i++) {                                                                    \
        ASSERT_EQ(expected[i], actual[i]);                                                                             \
    }

TEST(BinaryEncoder, EncodeFloat)
{
    SETUP(128)
    float f = 1.23;
    encoder.encode(f);
    uint8_t expected[] = { 0xFA, 0x3F, 0x9D, 0x70, 0xA4 };
    ASSERT_BUFFER_EQ(expected, buffer, encoder.getEncodedLength());
}

TEST(BinaryEncoder, EncodeArrayOfFloats)
{
    SETUP(128)
    std::array f = { 1.23f, 4.56f, 7.89f };
    encoder.encode(f);
    uint8_t expected[] = { 0x83, 0xFA, 0x3F, 0x9D, 0x70, 0xA4, 0xFA, 0x40,
                           0x91, 0xEB, 0x85, 0xFA, 0x40, 0xFC, 0x7A, 0xE1 };
    ASSERT_BUFFER_EQ(expected, buffer, encoder.getEncodedLength());
}

TEST(BinaryEncoder, EncodeString)
{
    SETUP(128)
    const char *hello = "world";
    encoder.encode(hello);
    uint8_t expected[] = { 0x65, 0x77, 0x6F, 0x72, 0x6C, 0x64 };
    ASSERT_BUFFER_EQ(expected, buffer, encoder.getEncodedLength());
}

TEST(BinaryEncoder, EncodeMap)
{
    SETUP(128)
    ASSERT_TRUE(encoder.encodeMapStart(2));
    ASSERT_TRUE(encoder.encode(0x01));
    ASSERT_TRUE(encoder.encode("hello"));
    ASSERT_TRUE(encoder.encode(0x02));
    std::array<uint32_t, 3> i32 = { { 1, 2, 3 } };
    ASSERT_TRUE(encoder.encode(i32));
    ASSERT_TRUE(encoder.encodeMapEnd(2));
    uint8_t expected[] = { 0xA2, 0x01, 0x65, 0x68, 0x65, 0x6C, 0x6C, 0x6F, 0x02, 0x83, 0x01, 0x02, 0x03 };
    ASSERT_BUFFER_EQ(expected, buffer, encoder.getEncodedLength());
}

TEST(BinaryEncoder, EncodeBag)
{
    SETUP(128)
    encoder.encodeList(1.23f, 123, "123");
    uint8_t expected[] = { 0x83, 0xFA, 0x3F, 0x9D, 0x70, 0xA4, 0x18, 0x7B, 0x63, 0x31, 0x32, 0x33 };
    ASSERT_BUFFER_EQ(expected, buffer, encoder.getEncodedLength());
}