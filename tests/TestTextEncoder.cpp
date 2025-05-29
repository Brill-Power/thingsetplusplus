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
    for (size_t i = 0; i < strlen(expected); i++) {                                                                    \
        ASSERT_EQ(expected[i], actual[i]);                                                                             \
    }

TEST(TextEncoder, EncodeFloat)
{
    SETUP(TEXT_ENCODER_MAX_NULL_TERMINATED_STRING_LENGTH)
    float f = 1.23F;
    encoder.encode(f);
    const char *expected = "1.230000";
    ASSERT_BUFFER_EQ(expected, encoder._rsp, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodeFloatPtr)
{
    SETUP(TEXT_ENCODER_MAX_NULL_TERMINATED_STRING_LENGTH)
    float f = 1.23F;
    float *ptr = &f;
    encoder.encode(ptr);
    const char *expected = "1.230000";
    ASSERT_BUFFER_EQ(expected, encoder._rsp, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodeArrayOfFloats)
{
    SETUP(TEXT_ENCODER_MAX_NULL_TERMINATED_STRING_LENGTH)
    std::array f = { 1.23F, 4.56F, 7.89F };
    encoder.encode(f);
    const char *expected = "[1.230000,4.560000,7.890000]";
    ASSERT_BUFFER_EQ(expected, encoder._rsp, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodeDouble)
{
    SETUP(TEXT_ENCODER_MAX_NULL_TERMINATED_STRING_LENGTH)
    double d = 1.23;
    encoder.encode(d);
    const char *expected = "1.230000";
    ASSERT_BUFFER_EQ(expected, encoder._rsp, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodeDoublePtr)
{
    SETUP(TEXT_ENCODER_MAX_NULL_TERMINATED_STRING_LENGTH)
    double d = 1.23;
    double *ptr = &d;
    encoder.encode(ptr);
    const char *expected = "1.230000";
    ASSERT_BUFFER_EQ(expected, encoder._rsp, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodeArrayOfDoubles)
{
    SETUP(TEXT_ENCODER_MAX_NULL_TERMINATED_STRING_LENGTH)
    std::array<double, 3> d = { 1.23, 4.56, 7.89 };
    encoder.encode(d);
    const char *expected = "[1.230000,4.560000,7.890000]";
    ASSERT_BUFFER_EQ(expected, encoder._rsp, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodeInt)
{
    SETUP(TEXT_ENCODER_MAX_NULL_TERMINATED_STRING_LENGTH)
    int i = 1;
    encoder.encode(i);
    const char *expected = "1";
    ASSERT_BUFFER_EQ(expected, encoder._rsp, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodeIntPtr)
{
    SETUP(TEXT_ENCODER_MAX_NULL_TERMINATED_STRING_LENGTH)
    int i = 1;
    int *ptr = &i;
    encoder.encode(ptr);
    const char *expected = "1";
    ASSERT_BUFFER_EQ(expected, encoder._rsp, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodeArrayOfInts)
{
    SETUP(TEXT_ENCODER_MAX_NULL_TERMINATED_STRING_LENGTH)
    std::array<int, 3> i = { 1, 2, 3 };
    encoder.encode(i);
    const char *expected = "[1,2,3]";
    ASSERT_BUFFER_EQ(expected, encoder._rsp, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodeInt8)
{
    SETUP(TEXT_ENCODER_MAX_NULL_TERMINATED_STRING_LENGTH)
    int8_t i = 1;
    encoder.encode(i);
    const char *expected = "1";
    ASSERT_BUFFER_EQ(expected, encoder._rsp, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodeInt8Ptr)
{
    SETUP(TEXT_ENCODER_MAX_NULL_TERMINATED_STRING_LENGTH)
    int8_t i = 1;
    int8_t *ptr = &i;
    encoder.encode(ptr);
    const char *expected = "1";
    ASSERT_BUFFER_EQ(expected, encoder._rsp, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodeArrayOfInt8s)
{
    SETUP(TEXT_ENCODER_MAX_NULL_TERMINATED_STRING_LENGTH)
    std::array<int8_t, 3> i = { 1, 2, 3 };
    encoder.encode(i);
    const char *expected = "[1,2,3]";
    ASSERT_BUFFER_EQ(expected, encoder._rsp, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodeInt16)
{
    SETUP(TEXT_ENCODER_MAX_NULL_TERMINATED_STRING_LENGTH)
    int16_t i = 1;
    encoder.encode(i);
    const char *expected = "1";
    ASSERT_BUFFER_EQ(expected, encoder._rsp, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodeInt16Ptr)
{
    SETUP(TEXT_ENCODER_MAX_NULL_TERMINATED_STRING_LENGTH)
    int16_t i = 1;
    int16_t *ptr = &i;
    encoder.encode(ptr);
    const char *expected = "1";
    ASSERT_BUFFER_EQ(expected, encoder._rsp, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodeArrayOfInt16s)
{
    SETUP(TEXT_ENCODER_MAX_NULL_TERMINATED_STRING_LENGTH)
    std::array<int16_t, 3> i = { 1, 2, 3 };
    encoder.encode(i);
    const char *expected = "[1,2,3]";
    ASSERT_BUFFER_EQ(expected, encoder._rsp, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodeInt32)
{
    SETUP(TEXT_ENCODER_MAX_NULL_TERMINATED_STRING_LENGTH)
    int32_t i = 1;
    encoder.encode(i);
    const char *expected = "1";
    ASSERT_BUFFER_EQ(expected, encoder._rsp, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodeInt32Ptr)
{
    SETUP(TEXT_ENCODER_MAX_NULL_TERMINATED_STRING_LENGTH)
    int32_t i = 1;
    int32_t *ptr = &i;
    encoder.encode(ptr);
    const char *expected = "1";
    ASSERT_BUFFER_EQ(expected, encoder._rsp, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodeArrayOfInt32s)
{
    SETUP(TEXT_ENCODER_MAX_NULL_TERMINATED_STRING_LENGTH)
    std::array<int32_t, 3> i = { 1, 2, 3 };
    encoder.encode(i);
    const char *expected = "[1,2,3]";
    ASSERT_BUFFER_EQ(expected, encoder._rsp, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodeInt64)
{
    SETUP(TEXT_ENCODER_MAX_NULL_TERMINATED_STRING_LENGTH)
    int64_t i = 1;
    encoder.encode(i);
    const char *expected = "1";
    ASSERT_BUFFER_EQ(expected, encoder._rsp, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodeInt64Ptr)
{
    SETUP(TEXT_ENCODER_MAX_NULL_TERMINATED_STRING_LENGTH)
    int64_t i = 1;
    int64_t *ptr = &i;
    encoder.encode(ptr);
    const char *expected = "1";
    ASSERT_BUFFER_EQ(expected, encoder._rsp, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodeArrayOfInt64s)
{
    SETUP(TEXT_ENCODER_MAX_NULL_TERMINATED_STRING_LENGTH)
    std::array<int64_t, 3> i = { 1, 2, 3 };
    encoder.encode(i);
    const char *expected = "[1,2,3]";
    ASSERT_BUFFER_EQ(expected, encoder._rsp, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodeUInt8)
{
    SETUP(TEXT_ENCODER_MAX_NULL_TERMINATED_STRING_LENGTH)
    uint8_t i = 1;
    encoder.encode(i);
    const char *expected = "1";
    ASSERT_BUFFER_EQ(expected, encoder._rsp, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodeUInt8Ptr)
{
    SETUP(TEXT_ENCODER_MAX_NULL_TERMINATED_STRING_LENGTH)
    uint8_t i = 1;
    uint8_t *ptr = &i;
    encoder.encode(ptr);
    const char *expected = "1";
    ASSERT_BUFFER_EQ(expected, encoder._rsp, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodeArrayOfUInt8s)
{
    SETUP(TEXT_ENCODER_MAX_NULL_TERMINATED_STRING_LENGTH)
    std::array<uint8_t, 3> i = { 1, 2, 3 };
    encoder.encode(i);
    const char *expected = "[1,2,3]";
    ASSERT_BUFFER_EQ(expected, encoder._rsp, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodeUInt16)
{
    SETUP(TEXT_ENCODER_MAX_NULL_TERMINATED_STRING_LENGTH)
    uint16_t i = 1;
    encoder.encode(i);
    const char *expected = "1";
    ASSERT_BUFFER_EQ(expected, encoder._rsp, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodeUInt16Ptr)
{
    SETUP(TEXT_ENCODER_MAX_NULL_TERMINATED_STRING_LENGTH)
    uint16_t i = 1;
    uint16_t *ptr = &i;
    encoder.encode(ptr);
    const char *expected = "1";
    ASSERT_BUFFER_EQ(expected, encoder._rsp, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodeArrayOfUInt16s)
{
    SETUP(TEXT_ENCODER_MAX_NULL_TERMINATED_STRING_LENGTH)
    std::array<uint16_t, 3> i = { 1, 2, 3 };
    encoder.encode(i);
    const char *expected = "[1,2,3]";
    ASSERT_BUFFER_EQ(expected, encoder._rsp, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodeUInt32)
{
    SETUP(TEXT_ENCODER_MAX_NULL_TERMINATED_STRING_LENGTH)
    uint32_t i = 1;
    encoder.encode(i);
    const char *expected = "1";
    ASSERT_BUFFER_EQ(expected, encoder._rsp, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodeUInt32Ptr)
{
    SETUP(TEXT_ENCODER_MAX_NULL_TERMINATED_STRING_LENGTH)
    uint32_t i = 1;
    uint32_t *ptr = &i;
    encoder.encode(ptr);
    const char *expected = "1";
    ASSERT_BUFFER_EQ(expected, encoder._rsp, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodeArrayOfUInt32s)
{
    SETUP(TEXT_ENCODER_MAX_NULL_TERMINATED_STRING_LENGTH)
    std::array<uint32_t, 3> i = { 1, 2, 3 };
    encoder.encode(i);
    const char *expected = "[1,2,3]";
    ASSERT_BUFFER_EQ(expected, encoder._rsp, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodeUInt64)
{
    SETUP(TEXT_ENCODER_MAX_NULL_TERMINATED_STRING_LENGTH)
    uint64_t i = 1;
    encoder.encode(i);
    const char *expected = "1";
    ASSERT_BUFFER_EQ(expected, encoder._rsp, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodeUInt64Ptr)
{
    SETUP(TEXT_ENCODER_MAX_NULL_TERMINATED_STRING_LENGTH)
    uint64_t i = 1;
    uint64_t *ptr = &i;
    encoder.encode(ptr);
    const char *expected = "1";
    ASSERT_BUFFER_EQ(expected, encoder._rsp, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodeArrayOfUInt64s)
{
    SETUP(TEXT_ENCODER_MAX_NULL_TERMINATED_STRING_LENGTH)
    std::array<uint64_t, 3> i = { 1, 2, 3 };
    encoder.encode(i);
    const char *expected = "[1,2,3]";
    ASSERT_BUFFER_EQ(expected, encoder._rsp, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodeBool)
{
    SETUP(TEXT_ENCODER_MAX_NULL_TERMINATED_STRING_LENGTH)
    bool b = true;
    encoder.encode(b);
    const char *expected = "1";
    ASSERT_BUFFER_EQ(expected, encoder._rsp, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodeBoolPtr)
{
    SETUP(TEXT_ENCODER_MAX_NULL_TERMINATED_STRING_LENGTH)
    bool b = true;
    bool *ptr = &b;
    encoder.encode(ptr);
    const char *expected = "1";
    ASSERT_BUFFER_EQ(expected, encoder._rsp, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodeArrayOfBools)
{
    SETUP(TEXT_ENCODER_MAX_NULL_TERMINATED_STRING_LENGTH)
    std::array<bool, 3> i = { true, false, true };
    encoder.encode(i);
    const char *expected = "[1,0,1]";
    ASSERT_BUFFER_EQ(expected, encoder._rsp, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodeString)
{
    SETUP(TEXT_ENCODER_MAX_NULL_TERMINATED_STRING_LENGTH)
    const char *hello = "world";
    encoder.encode(hello);
    const char *expected = "\"world\"";
    ASSERT_BUFFER_EQ(expected, encoder._rsp, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodeStdString)
{
    SETUP(TEXT_ENCODER_MAX_NULL_TERMINATED_STRING_LENGTH)
    const std::string bonjour = "monde";
    encoder.encode(bonjour);
    const char *expected = "\"monde\"";
    ASSERT_BUFFER_EQ(expected, encoder._rsp, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodeStdStringView)
{
    SETUP(TEXT_ENCODER_MAX_NULL_TERMINATED_STRING_LENGTH)
    const std::string_view hello = "hello";
    encoder.encode(hello);
    const char *expected = "\"hello\"";
    ASSERT_BUFFER_EQ(expected, encoder._rsp, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodePreamble)
{
    SETUP(TEXT_ENCODER_MAX_NULL_TERMINATED_STRING_LENGTH)
    encoder.encodePreamble();
    const char *expected = "";
    ASSERT_BUFFER_EQ(expected, encoder._rsp, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodeMap)
{
    SETUP(TEXT_ENCODER_MAX_NULL_TERMINATED_STRING_LENGTH)
    auto map = std::map<std::string, int>();
    map.insert_or_assign("hello", 1);
    map.insert_or_assign("world", 2);
    encoder.encode(map);
    const char *expected = "{\"hello\":1,\"world\":2}";
    ASSERT_BUFFER_EQ(expected, encoder._rsp, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodeList)
{
    SETUP(TEXT_ENCODER_MAX_NULL_TERMINATED_STRING_LENGTH)
    encoder.encodeList(1.23f, 123, "123");
    const char *expected = "[1.230000,123,\"123\"]";
    ASSERT_BUFFER_EQ(expected, encoder._rsp, encoder.getEncodedLength());
}