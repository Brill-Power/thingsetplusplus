/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "thingset++/ThingSet.hpp"
#include "thingset++/ThingSetGroup.hpp"
#include "thingset++/ThingSetProperty.hpp"
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
    SETUP(TEXT_ENCODER_BUFFER_SIZE)
    float f = 1.23F;
    encoder.encode(f);
    const char *expected = "1.230000";
    ASSERT_BUFFER_EQ(expected, buffer, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodeFloatPtr)
{
    SETUP(TEXT_ENCODER_BUFFER_SIZE)
    float f = 1.23F;
    float *ptr = &f;
    encoder.encode(ptr);
    const char *expected = "1.230000";
    ASSERT_BUFFER_EQ(expected, buffer, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodeArrayOfFloats)
{
    SETUP(TEXT_ENCODER_BUFFER_SIZE)
    std::array f = { 1.23F, 4.56F, 7.89F };
    encoder.encode(f);
    const char *expected = "[1.230000,4.560000,7.890000]";
    ASSERT_BUFFER_EQ(expected, buffer, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodeDouble)
{
    SETUP(TEXT_ENCODER_BUFFER_SIZE)
    double d = 1.23;
    encoder.encode(d);
    const char *expected = "1.230000";
    ASSERT_BUFFER_EQ(expected, buffer, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodeDoublePtr)
{
    SETUP(TEXT_ENCODER_BUFFER_SIZE)
    double d = 1.23;
    double *ptr = &d;
    encoder.encode(ptr);
    const char *expected = "1.230000";
    ASSERT_BUFFER_EQ(expected, buffer, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodeArrayOfDoubles)
{
    SETUP(TEXT_ENCODER_BUFFER_SIZE)
    std::array<double, 3> d = { 1.23, 4.56, 7.89 };
    encoder.encode(d);
    const char *expected = "[1.230000,4.560000,7.890000]";
    ASSERT_BUFFER_EQ(expected, buffer, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodeInt)
{
    SETUP(TEXT_ENCODER_BUFFER_SIZE)
    int i = 1;
    encoder.encode(i);
    const char *expected = "1";
    ASSERT_BUFFER_EQ(expected, buffer, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodeIntPtr)
{
    SETUP(TEXT_ENCODER_BUFFER_SIZE)
    int i = 1;
    int *ptr = &i;
    encoder.encode(ptr);
    const char *expected = "1";
    ASSERT_BUFFER_EQ(expected, buffer, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodeArrayOfInts)
{
    SETUP(TEXT_ENCODER_BUFFER_SIZE)
    std::array<int, 3> i = { 1, 2, 3 };
    encoder.encode(i);
    const char *expected = "[1,2,3]";
    ASSERT_BUFFER_EQ(expected, buffer, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodeInt8)
{
    SETUP(TEXT_ENCODER_BUFFER_SIZE)
    int8_t i = 1;
    encoder.encode(i);
    const char *expected = "1";
    ASSERT_BUFFER_EQ(expected, buffer, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodeInt8Ptr)
{
    SETUP(TEXT_ENCODER_BUFFER_SIZE)
    int8_t i = 1;
    int8_t *ptr = &i;
    encoder.encode(ptr);
    const char *expected = "1";
    ASSERT_BUFFER_EQ(expected, buffer, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodeArrayOfInt8s)
{
    SETUP(TEXT_ENCODER_BUFFER_SIZE)
    std::array<int8_t, 3> i = { 1, 2, 3 };
    encoder.encode(i);
    const char *expected = "[1,2,3]";
    ASSERT_BUFFER_EQ(expected, buffer, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodeInt16)
{
    SETUP(TEXT_ENCODER_BUFFER_SIZE)
    int16_t i = 1;
    encoder.encode(i);
    const char *expected = "1";
    ASSERT_BUFFER_EQ(expected, buffer, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodeInt16Ptr)
{
    SETUP(TEXT_ENCODER_BUFFER_SIZE)
    int16_t i = 1;
    int16_t *ptr = &i;
    encoder.encode(ptr);
    const char *expected = "1";
    ASSERT_BUFFER_EQ(expected, buffer, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodeArrayOfInt16s)
{
    SETUP(TEXT_ENCODER_BUFFER_SIZE)
    std::array<int16_t, 3> i = { 1, 2, 3 };
    encoder.encode(i);
    const char *expected = "[1,2,3]";
    ASSERT_BUFFER_EQ(expected, buffer, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodeInt32)
{
    SETUP(TEXT_ENCODER_BUFFER_SIZE)
    int32_t i = 1;
    encoder.encode(i);
    const char *expected = "1";
    ASSERT_BUFFER_EQ(expected, buffer, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodeInt32Ptr)
{
    SETUP(TEXT_ENCODER_BUFFER_SIZE)
    int32_t i = 1;
    int32_t *ptr = &i;
    encoder.encode(ptr);
    const char *expected = "1";
    ASSERT_BUFFER_EQ(expected, buffer, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodeArrayOfInt32s)
{
    SETUP(TEXT_ENCODER_BUFFER_SIZE)
    std::array<int32_t, 3> i = { 1, 2, 3 };
    encoder.encode(i);
    const char *expected = "[1,2,3]";
    ASSERT_BUFFER_EQ(expected, buffer, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodeInt64)
{
    SETUP(TEXT_ENCODER_BUFFER_SIZE)
    int64_t i = 1;
    encoder.encode(i);
    const char *expected = "1";
    ASSERT_BUFFER_EQ(expected, buffer, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodeInt64Ptr)
{
    SETUP(TEXT_ENCODER_BUFFER_SIZE)
    int64_t i = 1;
    int64_t *ptr = &i;
    encoder.encode(ptr);
    const char *expected = "1";
    ASSERT_BUFFER_EQ(expected, buffer, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodeArrayOfInt64s)
{
    SETUP(TEXT_ENCODER_BUFFER_SIZE)
    std::array<int64_t, 3> i = { 1, 2, 3 };
    encoder.encode(i);
    const char *expected = "[1,2,3]";
    ASSERT_BUFFER_EQ(expected, buffer, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodeUInt8)
{
    SETUP(TEXT_ENCODER_BUFFER_SIZE)
    uint8_t i = 1;
    encoder.encode(i);
    const char *expected = "1";
    ASSERT_BUFFER_EQ(expected, buffer, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodeUInt8Ptr)
{
    SETUP(TEXT_ENCODER_BUFFER_SIZE)
    uint8_t i = 1;
    uint8_t *ptr = &i;
    encoder.encode(ptr);
    const char *expected = "1";
    ASSERT_BUFFER_EQ(expected, buffer, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodeArrayOfUInt8s)
{
    SETUP(TEXT_ENCODER_BUFFER_SIZE)
    std::array<uint8_t, 3> i = { 1, 2, 3 };
    encoder.encode(i);
    const char *expected = "[1,2,3]";
    ASSERT_BUFFER_EQ(expected, buffer, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodeUInt16)
{
    SETUP(TEXT_ENCODER_BUFFER_SIZE)
    uint16_t i = 1;
    encoder.encode(i);
    const char *expected = "1";
    ASSERT_BUFFER_EQ(expected, buffer, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodeUInt16Ptr)
{
    SETUP(TEXT_ENCODER_BUFFER_SIZE)
    uint16_t i = 1;
    uint16_t *ptr = &i;
    encoder.encode(ptr);
    const char *expected = "1";
    ASSERT_BUFFER_EQ(expected, buffer, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodeArrayOfUInt16s)
{
    SETUP(TEXT_ENCODER_BUFFER_SIZE)
    std::array<uint16_t, 3> i = { 1, 2, 3 };
    encoder.encode(i);
    const char *expected = "[1,2,3]";
    ASSERT_BUFFER_EQ(expected, buffer, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodeUInt32)
{
    SETUP(TEXT_ENCODER_BUFFER_SIZE)
    uint32_t i = 1;
    encoder.encode(i);
    const char *expected = "1";
    ASSERT_BUFFER_EQ(expected, buffer, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodeUInt32Ptr)
{
    SETUP(TEXT_ENCODER_BUFFER_SIZE)
    uint32_t i = 1;
    uint32_t *ptr = &i;
    encoder.encode(ptr);
    const char *expected = "1";
    ASSERT_BUFFER_EQ(expected, buffer, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodeArrayOfUInt32s)
{
    SETUP(TEXT_ENCODER_BUFFER_SIZE)
    std::array<uint32_t, 3> i = { 1, 2, 3 };
    encoder.encode(i);
    const char *expected = "[1,2,3]";
    ASSERT_BUFFER_EQ(expected, buffer, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodeUInt64)
{
    SETUP(TEXT_ENCODER_BUFFER_SIZE)
    uint64_t i = 1;
    encoder.encode(i);
    const char *expected = "1";
    ASSERT_BUFFER_EQ(expected, buffer, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodeUInt64Ptr)
{
    SETUP(TEXT_ENCODER_BUFFER_SIZE)
    uint64_t i = 1;
    uint64_t *ptr = &i;
    encoder.encode(ptr);
    const char *expected = "1";
    ASSERT_BUFFER_EQ(expected, buffer, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodeArrayOfUInt64s)
{
    SETUP(TEXT_ENCODER_BUFFER_SIZE)
    std::array<uint64_t, 3> i = { 1, 2, 3 };
    encoder.encode(i);
    const char *expected = "[1,2,3]";
    ASSERT_BUFFER_EQ(expected, buffer, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodeBool)
{
    SETUP(TEXT_ENCODER_BUFFER_SIZE)
    bool b = true;
    encoder.encode(b);
    const char *expected = "1";
    ASSERT_BUFFER_EQ(expected, buffer, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodeBoolPtr)
{
    SETUP(TEXT_ENCODER_BUFFER_SIZE)
    bool b = true;
    bool *ptr = &b;
    encoder.encode(ptr);
    const char *expected = "1";
    ASSERT_BUFFER_EQ(expected, buffer, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodeArrayOfBools)
{
    SETUP(TEXT_ENCODER_BUFFER_SIZE)
    std::array<bool, 3> i = { true, false, true };
    encoder.encode(i);
    const char *expected = "[1,0,1]";
    ASSERT_BUFFER_EQ(expected, buffer, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodeString)
{
    SETUP(TEXT_ENCODER_BUFFER_SIZE)
    const char *hello = "world";
    encoder.encode(hello);
    const char *expected = "\"world\"";
    ASSERT_BUFFER_EQ(expected, buffer, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodeStdString)
{
    SETUP(TEXT_ENCODER_BUFFER_SIZE)
    const std::string bonjour = "monde";
    encoder.encode(bonjour);
    const char *expected = "\"monde\"";
    ASSERT_BUFFER_EQ(expected, buffer, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodeStdStringView)
{
    SETUP(TEXT_ENCODER_BUFFER_SIZE)
    const std::string_view hello = "hello";
    encoder.encode(hello);
    const char *expected = "\"hello\"";
    ASSERT_BUFFER_EQ(expected, buffer, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodeNull)
{
    SETUP(TEXT_ENCODER_BUFFER_SIZE)
    encoder.encodeNull();
    const char *expected = "null";
    ASSERT_BUFFER_EQ(expected, buffer, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodePreamble)
{
    SETUP(TEXT_ENCODER_BUFFER_SIZE)
    encoder.encodePreamble();
    const char *expected = "";
    ASSERT_BUFFER_EQ(expected, buffer, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodeMap)
{
    SETUP(TEXT_ENCODER_BUFFER_SIZE)
    auto map = std::map<std::string, int>();
    map.insert_or_assign("hello", 1);
    map.insert_or_assign("world", 2);
    encoder.encode(map);
    const char *expected = "{\"hello\":1,\"world\":2}";
    ASSERT_BUFFER_EQ(expected, buffer, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodeList)
{
    SETUP(TEXT_ENCODER_BUFFER_SIZE)
    encoder.encodeList(1.23f, 123, "123");
    const char *expected = "[1.230000,123,\"123\"]";
    ASSERT_BUFFER_EQ(expected, buffer, encoder.getEncodedLength());
}

namespace {
template <size_t N>
void encode_nested_array(ThingSetTextEncoder &encoder, const std::array<int, N> &arr)
{
    encoder.encodeMapStart(1);
    encoder.encode(std::make_pair(std::string_view{"a"}, arr));
    encoder.encodeMapEnd(1);
}
} // namespace

TEST(TextEncoder, AbbreviateTruncatesNestedArray)
{
    char buffer[TEXT_ENCODER_BUFFER_SIZE];
    ThingSetTextEncoder encoder(buffer, sizeof(buffer), TextEncoderOptions::abbreviateArrays);
    std::array<int, 8> arr{ 1, 2, 3, 4, 5, 6, 7, 8 };
    encode_nested_array(encoder, arr);
    const char *expected = "{\"a\":[1,2,3,...]}";
    ASSERT_BUFFER_EQ(expected, buffer, encoder.getEncodedLength());
}

TEST(TextEncoder, AbbreviateLeavesShortArrayAlone)
{
    char buffer[TEXT_ENCODER_BUFFER_SIZE];
    ThingSetTextEncoder encoder(buffer, sizeof(buffer), TextEncoderOptions::abbreviateArrays);
    std::array<int, 3> arr{ 1, 2, 3 };
    encode_nested_array(encoder, arr);
    const char *expected = "{\"a\":[1,2,3]}";
    ASSERT_BUFFER_EQ(expected, buffer, encoder.getEncodedLength());
}

TEST(TextEncoder, NoAbbreviateMeansNoTruncation)
{
    char buffer[TEXT_ENCODER_BUFFER_SIZE];
    ThingSetTextEncoder encoder(buffer, sizeof(buffer));  // default: none
    std::array<int, 8> arr{ 1, 2, 3, 4, 5, 6, 7, 8 };
    encode_nested_array(encoder, arr);
    const char *expected = "{\"a\":[1,2,3,4,5,6,7,8]}";
    ASSERT_BUFFER_EQ(expected, buffer, encoder.getEncodedLength());
}

TEST(TextEncoder, AbbreviateDoesNotTruncateTopLevelArray)
{
    char buffer[TEXT_ENCODER_BUFFER_SIZE];
    ThingSetTextEncoder encoder(buffer, sizeof(buffer), TextEncoderOptions::abbreviateArrays);
    std::array<int, 8> arr{ 1, 2, 3, 4, 5, 6, 7, 8 };
    encoder.encode(arr);
    const char *expected = "[1,2,3,4,5,6,7,8]";
    ASSERT_BUFFER_EQ(expected, buffer, encoder.getEncodedLength());
}

TEST(TextEncoder, DisplayFriendlyTruncatesAndOutlines)
{
    ThingSetGroup<0xA040, 0, "outer"> outer;
    ThingSetReadOnlyProperty<int> outerLeaf{ 0xA041, 0xA040, "ol" };
    ThingSetGroup<0xA042, 0xA040, "sub"> sub;
    ThingSetReadOnlyProperty<int> subLeaf{ 0xA043, 0xA042, "sl" };
    outerLeaf = 1;
    subLeaf = 2;

    char buffer[256];
    ThingSetTextEncoder encoder(buffer, sizeof(buffer), TextEncoderOptions::displayFriendly);
    outer.encode(encoder);
    const char *expected = "{\"ol\":1,\"sub\":{}}";
    ASSERT_BUFFER_EQ(expected, buffer, encoder.getEncodedLength());
}

TEST(TextEncoder, TestBoundaryEncode)
{
    SETUP(1) // make buffer of size 1 to ensure value cannot fit
    encoder.encode(1.23f);
    const char *expected = "";
    ASSERT_BUFFER_EQ(expected, buffer, encoder.getEncodedLength());
}

// Empty containers previously produced broken output (encodeMapEnd/encodeListEnd
// unconditionally stripped the character before the closing brace, eating the
// opening brace when no trailing separator existed). These two tests lock in the fix.
TEST(TextEncoder, EncodeEmptyMap)
{
    SETUP(TEXT_ENCODER_BUFFER_SIZE)
    encoder.encodeMapStart(0);
    encoder.encodeMapEnd(0);
    const char *expected = "{}";
    ASSERT_BUFFER_EQ(expected, buffer, encoder.getEncodedLength());
}

TEST(TextEncoder, EncodeEmptyList)
{
    SETUP(TEXT_ENCODER_BUFFER_SIZE)
    encoder.encodeListStart(0);
    encoder.encodeListEnd(0);
    const char *expected = "[]";
    ASSERT_BUFFER_EQ(expected, buffer, encoder.getEncodedLength());
}

TEST(TextEncoder, GroupEncodesFullByDefault)
{
    // IDs 0xA000+ chosen to avoid collisions with other tests.
    ThingSetGroup<0xA000, 0, "topG"> top;
    ThingSetReadOnlyProperty<int> leaf{ 0xA001, 0xA000, "leaf" };
    leaf = 42;

    char buffer[128];
    ThingSetTextEncoder encoder(buffer, sizeof(buffer));  // default: TextEncoderOptions::none

    top.encode(encoder);
    const char *expected = "{\"leaf\":42}";
    ASSERT_BUFFER_EQ(expected, buffer, encoder.getEncodedLength());
}

TEST(TextEncoder, OutlineOnlyAppliesToNestedGroups)
{
    ThingSetGroup<0xA010, 0, "outer"> outer;
    ThingSetReadOnlyProperty<int> outerLeaf{ 0xA011, 0xA010, "ol" };
    ThingSetGroup<0xA012, 0xA010, "sub"> sub;
    ThingSetReadOnlyProperty<int> subLeaf{ 0xA013, 0xA012, "sl" };
    ThingSetGroup<0xA014, 0xA012, "leaf2"> deeper;
    outerLeaf = 1;
    subLeaf = 2;

    char buffer[128];
    ThingSetTextEncoder encoder(buffer, sizeof(buffer), TextEncoderOptions::outlineGroups);

    outer.encode(encoder);
    const char *expected = "{\"ol\":1,\"sub\":{\"leaf2\":{}}}";
    ASSERT_BUFFER_EQ(expected, buffer, encoder.getEncodedLength());
}

TEST(TextEncoder, OutlineRecursesThroughNestedGroups)
{
    ThingSetGroup<0xA020, 0, "L0"> l0;
    ThingSetReadOnlyProperty<int> leaf0{ 0xA021, 0xA020, "x" };
    ThingSetGroup<0xA022, 0xA020, "L1"> l1;
    ThingSetReadOnlyProperty<int> leaf1{ 0xA023, 0xA022, "x" };
    ThingSetGroup<0xA024, 0xA022, "L2"> l2;
    ThingSetReadOnlyProperty<int> leaf2{ 0xA025, 0xA024, "x" };
    ThingSetGroup<0xA026, 0xA024, "L3"> l3;
    leaf0 = 0;
    leaf1 = 0;
    leaf2 = 0;

    char buffer[256];
    ThingSetTextEncoder encoder(buffer, sizeof(buffer), TextEncoderOptions::outlineGroups);

    l0.encode(encoder);
    const char *expected = "{\"x\":0,\"L1\":{\"L2\":{\"L3\":{}}}}";
    ASSERT_BUFFER_EQ(expected, buffer, encoder.getEncodedLength());
}

TEST(TextEncoder, DisplayFriendlyImpliesOutline)
{
    ThingSetGroup<0xA030, 0, "outer"> outer;
    ThingSetGroup<0xA031, 0xA030, "sub"> sub;
    ThingSetReadOnlyProperty<int> subLeaf{ 0xA032, 0xA031, "sl" };
    subLeaf = 7;

    char buffer[128];
    ThingSetTextEncoder encoder(buffer, sizeof(buffer), TextEncoderOptions::displayFriendly);

    outer.encode(encoder);
    const char *expected = "{\"sub\":{}}";
    ASSERT_BUFFER_EQ(expected, buffer, encoder.getEncodedLength());
}