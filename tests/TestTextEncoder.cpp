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

TEST(TextEncoder, EncodeArrayOfFloats)
{
    SETUP(256)
    std::array f = { 1.23F, 4.56F, 7.89F };
    encoder.encode(f);
    const char *expected = "[1.230000,4.560000,7.890000]";
    ASSERT_BUFFER_EQ(expected, encoder._rsp, encoder.getEncodedLength());
}

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

TEST(TextEncoder, EncodeMap)
{
    SETUP(256)
    auto map = std::map<std::string, int>();
    map.insert_or_assign("hello", 1);
    map.insert_or_assign("world", 2);
    encoder.encode(map);
    const char *expected = "{\"hello\":1,\"world\":2}";
    ASSERT_BUFFER_EQ(expected, encoder._rsp, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodeList)
{
    SETUP(256)
    encoder.encodeList(1.23f, 123, "123");
    const char *expected = "[1.230000,123,\"123\"]";
    ASSERT_BUFFER_EQ(expected, encoder._rsp, encoder.getEncodedLength());
}