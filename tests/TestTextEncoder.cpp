/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "thingset++/ThingSetTextEncoder.hpp"
#include "gtest/gtest.h"

using namespace ThingSet;

#define SETUP(size)                                                                                                    \
    char buffer[size];                                                                                                 \
    ThingSetTextEncoder encoder(buffer, size);

#define ASSERT_BUFFER_EQ(expected, actual, actual_len)                                                                 \
    ASSERT_EQ(strlen(expected), actual_len);                                                                           \
    for (size_t i = 0; i < sizeof(expected); i++) {                                                                    \
        printf("value: %c", actual[i]);                                                                                \
        ASSERT_EQ(expected[i], actual[i]);                                                                             \
    }

TEST(TextEncoder, EncodeFloat)
{
    SETUP(256)
    float f = 1.23F;
    encoder.encode(f);
    const char *expected = "1.230000";
    ASSERT_BUFFER_EQ(expected, encoder._rsp, encoder.getEncodedLength());
}

// todo reimplement this
// TEST(TextEncoder, EncodeArrayOfFloats)
// {
//     SETUP(256)
//     std::array f = { 1.23f, 4.56f, 7.89f };
//     encoder.encode(f);
//     uint8_t expected[] = { 0x83, 0xFA, 0x3F, 0x9D, 0x70, 0xA4, 0xFA, 0x40,
//                            0x91, 0xEB, 0x85, 0xFA, 0x40, 0xFC, 0x7A, 0xE1 };
//     ASSERT_BUFFER_EQ(expected, buffer, encoder.getEncodedLength());
// }

TEST(TextEncoder, EncodeString)
{
    SETUP(256)
    const char *hello = "world";
    encoder.encode(hello);
    const char *expected = "\"world\"";
    ASSERT_BUFFER_EQ(expected, encoder._rsp, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodeStdString)
{
    SETUP(256)
    const std::string bonjour = "monde";
    encoder.encode(bonjour);
    const char *expected = "\"monde\"";
    ASSERT_BUFFER_EQ(expected, encoder._rsp, encoder.getEncodedLength());
}

// todo reimplement this
// TEST(TextEncoder, EncodeMap)
// {
//     SETUP(256)
//     ASSERT_TRUE(encoder.encodeMapStart(2));
//     ASSERT_TRUE(encoder.encode(0x01));
//     ASSERT_TRUE(encoder.encode("hello"));
//     ASSERT_TRUE(encoder.encode(0x02));
//     std::array<uint32_t, 3> i32 = { { 1, 2, 3 } };
//     ASSERT_TRUE(encoder.encode(i32));
//     ASSERT_TRUE(encoder.encodeMapEnd(2));
//     uint8_t expected[] = { 0xA2, 0x01, 0x65, 0x68, 0x65, 0x6C, 0x6C, 0x6F, 0x02, 0x83, 0x01, 0x02, 0x03 };
//     ASSERT_BUFFER_EQ(expected, buffer, encoder.getEncodedLength());
// }

// todo reimplement this
// TEST(TextEncoder, EncodeBag)
// {
//     SETUP(256)
//     encoder.encodeList(1.23f, 123, "123");
//     uint8_t expected[] = { 0x83, 0xFA, 0x3F, 0x9D, 0x70, 0xA4, 0x18, 0x7B, 0x63, 0x31, 0x32, 0x33 };
//     ASSERT_BUFFER_EQ(expected, buffer, encoder.getEncodedLength());
// }