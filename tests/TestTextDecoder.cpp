/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "thingset++/ThingSetTextDecoder.hpp"
#include "gtest/gtest.h"

using namespace ThingSet;

TEST(TextDecoder, DecodeStdString)
{
    char buffer[] = "\"Hello World\"";
    ThingSetTextDecoder decoder(buffer, strlen(buffer));
    std::string value;
    std::string *valuePtr = &value;
    bool ret = decoder.decode(valuePtr);
    ASSERT_TRUE(ret);
    ASSERT_EQ(value, "Hello World");
}

TEST(TextDecoder, DecodeChar)
{
    char buffer[] = "F";
    ThingSetTextDecoder decoder(buffer, strlen(buffer));
    char value;
    char *valuePtr = &value;
    bool ret = decoder.decode(valuePtr);
    ASSERT_TRUE(ret);
    ASSERT_EQ(value, 'F');
}

TEST(TextDecoder, DecodeDouble)
{
    char buffer[] = "12.4";
    ThingSetTextDecoder decoder(buffer, strlen(buffer));
    double value;
    double *valuePtr = &value;
    bool ret = decoder.decode(valuePtr);
    ASSERT_TRUE(ret);
    ASSERT_NEAR(*valuePtr, 12.4F, 1e-6);
}

TEST(TextDecoder, DecodeFloat)
{
    char buffer[] = "12.4";
    ThingSetTextDecoder decoder(buffer, strlen(buffer));
    float value;
    float *valuePtr = &value;
    bool ret = decoder.decode(valuePtr);
    ASSERT_TRUE(ret);
    ASSERT_NEAR(*valuePtr, 12.4F, 1e-6);
}

TEST(TextDecoder, DecodeUint8)
{
    char buffer[] = "255";
    ThingSetTextDecoder decoder(buffer, strlen(buffer));
    uint8_t value;
    uint8_t *valuePtr = &value;
    bool ret = decoder.decode(valuePtr);
    ASSERT_TRUE(ret);
    ASSERT_EQ(value, UINT8_MAX);
}

TEST(TextDecoder, DecodeUint16)
{
    char buffer[] = "65535";
    ThingSetTextDecoder decoder(buffer, strlen(buffer));
    uint16_t value;
    uint16_t *valuePtr = &value;
    bool ret = decoder.decode(valuePtr);
    ASSERT_TRUE(ret);
    ASSERT_EQ(value, UINT16_MAX);
}

TEST(TextDecoder, DecodeUint32)
{
    char buffer[] = "4294967295";
    ThingSetTextDecoder decoder(buffer, strlen(buffer));
    uint32_t value;
    uint32_t *valuePtr = &value;
    bool ret = decoder.decode(valuePtr);
    ASSERT_TRUE(ret);
    ASSERT_EQ(value, UINT32_MAX);
}

TEST(TextDecoder, DecodeUint64)
{
    char buffer[] = "18446744073709551615";
    ThingSetTextDecoder decoder(buffer, strlen(buffer));
    uint64_t value;
    uint64_t *valuePtr = &value;
    bool ret = decoder.decode(valuePtr);
    ASSERT_TRUE(ret);
    ASSERT_EQ(value, UINT64_MAX);
}

TEST(TextDecoder, DecodeInt8)
{
    char buffer[] = "-128";
    ThingSetTextDecoder decoder(buffer, strlen(buffer));
    int8_t value;
    int8_t *valuePtr = &value;
    bool ret = decoder.decode(valuePtr);
    ASSERT_TRUE(ret);
    ASSERT_EQ(value, INT8_MIN);
}

TEST(TextDecoder, DecodeInt16)
{
    char buffer[] = "-32768";
    ThingSetTextDecoder decoder(buffer, strlen(buffer));
    int16_t value;
    int16_t *valuePtr = &value;
    bool ret = decoder.decode(valuePtr);
    ASSERT_TRUE(ret);
    ASSERT_EQ(value, INT16_MIN);
}

TEST(TextDecoder, DecodeInt32)
{
    char buffer[] = "-2147483648";
    ThingSetTextDecoder decoder(buffer, strlen(buffer));
    int32_t value;
    int32_t *valuePtr = &value;
    bool ret = decoder.decode(valuePtr);
    ASSERT_TRUE(ret);
    ASSERT_EQ(value, INT32_MIN);
}

TEST(TextDecoder, DecodeInt64)
{
    char buffer[] = "-9223372036854775808";
    ThingSetTextDecoder decoder(buffer, strlen(buffer));
    uint64_t value;
    uint64_t *valuePtr = &value;
    bool ret = decoder.decode(valuePtr);
    ASSERT_TRUE(ret);
    ASSERT_EQ(value, INT64_MIN);
}

TEST(TextDecoder, DecodeList)
{
    char buffer[] = "[1234567890,9876543210]";
    ThingSetTextDecoder decoder(buffer, strlen(buffer));
    std::tuple<uint64_t, uint64_t> values;
    bool ret = decoder.decodeList(values);
    ASSERT_TRUE(ret);
    ASSERT_EQ(std::get<0>(values), 1234567890);
    ASSERT_EQ(std::get<1>(values), 9876543210);
}

// todo implement more tests

// todo reimplement or delete everything below

// TEST(TextDecoder, DecodeArrayOfFloats)
// {
//     uint8_t buffer[] = {
//         0x83, 0xFA, 0x3F, 0x9D, 0x70, 0xA4, 0xFA, 0x40, 0x91, 0xEB, 0x85, 0xFA, 0x40, 0xFC, 0x7A, 0xE1
//     };
//     FixedDepthThingSetTextDecoder decoder(buffer, sizeof(buffer));
//     std::array<float, 3> three;
//     ASSERT_TRUE(decoder.decode(&three));
// }

// TEST(TextDecoder, FailToDecodeUndersizeArray)
// {
//     uint8_t buffer[] = {
//         0x83, 0xFA, 0x3F, 0x9D, 0x70, 0xA4, 0xFA, 0x40, 0x91, 0xEB, 0x85, 0xFA, 0x40, 0xFC, 0x7A, 0xE1
//     };
//     FixedDepthThingSetTextDecoder decoder(buffer, sizeof(buffer));
//     std::array<float, 4> four;
//     ASSERT_FALSE(decoder.decode(&four));
// }

// TEST(TextDecoder, SkipUndersizeArrayAndSuccessfullyDecodeNextElement)
// {
//     uint8_t buffer[] = { 0x82, 0x83, 0xFA, 0x3F, 0x9D, 0x70, 0xA4, 0xFA, 0x40, 0x91,
//                          0xEB, 0x85, 0xFA, 0x40, 0xFC, 0x7A, 0xE1, 0x19, 0x60, 0x7b };
//     FixedDepthThingSetTextDecoder decoder(buffer, sizeof(buffer));
//     ASSERT_TRUE(decoder.decodeListStart());
//     std::array<float, 4> four;
//     ASSERT_FALSE(decoder.decode(&four));
//     ASSERT_TRUE(decoder.skip());
//     uint16_t court;
//     ASSERT_TRUE(decoder.decode(&court));
//     ASSERT_EQ(0x607b, court);
// }

// TEST(TextDecoder, DecodeUndersizeArray)
// {
//     uint8_t buffer[] = {
//         0x83, 0xFA, 0x3F, 0x9D, 0x70, 0xA4, 0xFA, 0x40, 0x91, 0xEB, 0x85, 0xFA, 0x40, 0xFC, 0x7A, 0xE1
//     };
//     FixedDepthThingSetTextDecoder decoder(buffer, sizeof(buffer),
//                                             ThingSetTextDecoderOptions::allowUndersizedArrays);
//     std::array<float, 4> four;
//     ASSERT_TRUE(decoder.decode(&four));
//     ASSERT_EQ(1.23f, four[0]);
//     ASSERT_EQ(4.56f, four[1]);
// }

// TEST(TextDecoder, DecodeMap)
// {
//     uint8_t buffer[] = { 0xA3, 0x18, 0x1D, 0x70, 0x45, 0x39, 0x33, 0x41, 0x31, 0x34, 0x32, 0x42, 0x32, 0x38, 0x32,
//                          0x43, 0x34, 0x41, 0x44, 0x30, 0x19, 0x02, 0x8c, 0x10, 0x19, 0x60, 0x60, 0x83, 0xFA, 0x3F,
//                          0x9D, 0x70, 0xA4, 0xFA, 0x40, 0x91, 0xEB, 0x85, 0xFA, 0x40, 0xFC, 0x7A, 0xE1 };
//     FixedDepthThingSetTextDecoder decoder(buffer, sizeof(buffer));
//     char nodeId[17];
//     uint8_t canAddr;
//     std::array<float, 3> three;
//     ASSERT_TRUE(decoder.decodeMap<uint16_t>([&](uint16_t &key) {
//         switch (key) {
//             case 0x1D:
//                 return decoder.decode(nodeId);
//             case 0x028C:
//                 return decoder.decode(&canAddr);
//             case 0x6060:
//                 return decoder.decode(&three);
//             default:
//                 return false;
//         }
//     }));
//     ASSERT_STREQ("E93A142B282C4AD0", nodeId);
//     ASSERT_EQ(0x10, canAddr);
//     ASSERT_EQ(1.23f, three[0]);
// }