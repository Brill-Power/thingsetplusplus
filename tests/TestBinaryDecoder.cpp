/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "thingset++/ThingSetBinaryDecoder.hpp"
#include "gtest/gtest.h"

using namespace ThingSet;

TEST(BinaryDecoder, DecodeArrayOfFloats)
{
    uint8_t buffer[] = {
        0x83, 0xFA, 0x3F, 0x9D, 0x70, 0xA4, 0xFA, 0x40, 0x91, 0xEB, 0x85, 0xFA, 0x40, 0xFC, 0x7A, 0xE1
    };
    FixedDepthThingSetBinaryDecoder decoder(buffer, sizeof(buffer));
    std::array<float, 3> three;
    ASSERT_TRUE(decoder.decode(&three));
}

TEST(BinaryDecoder, FailToDecodeUndersizeArray)
{
    uint8_t buffer[] = {
        0x83, 0xFA, 0x3F, 0x9D, 0x70, 0xA4, 0xFA, 0x40, 0x91, 0xEB, 0x85, 0xFA, 0x40, 0xFC, 0x7A, 0xE1
    };
    FixedDepthThingSetBinaryDecoder decoder(buffer, sizeof(buffer));
    std::array<float, 4> four;
    ASSERT_FALSE(decoder.decode(&four));
}

TEST(BinaryDecoder, SkipUndersizeArrayAndSuccessfullyDecodeNextElement)
{
    uint8_t buffer[] = { 0x82, 0x83, 0xFA, 0x3F, 0x9D, 0x70, 0xA4, 0xFA, 0x40, 0x91,
                         0xEB, 0x85, 0xFA, 0x40, 0xFC, 0x7A, 0xE1, 0x19, 0x60, 0x7b };
    FixedDepthThingSetBinaryDecoder decoder(buffer, sizeof(buffer));
    ASSERT_TRUE(decoder.decodeListStart());
    std::array<float, 4> four;
    ASSERT_FALSE(decoder.decode(&four));
    ASSERT_TRUE(decoder.skip());
    uint16_t court;
    ASSERT_TRUE(decoder.decode(&court));
    ASSERT_EQ(0x607b, court);
}

TEST(BinaryDecoder, DecodeUndersizeArray)
{
    uint8_t buffer[] = {
        0x83, 0xFA, 0x3F, 0x9D, 0x70, 0xA4, 0xFA, 0x40, 0x91, 0xEB, 0x85, 0xFA, 0x40, 0xFC, 0x7A, 0xE1
    };
    FixedDepthThingSetBinaryDecoder decoder(buffer, sizeof(buffer),
                                            ThingSetBinaryDecoderOptions::allowUndersizedArrays);
    std::array<float, 4> four;
    ASSERT_TRUE(decoder.decode(&four));
    ASSERT_EQ(1.23f, four[0]);
    ASSERT_EQ(4.56f, four[1]);
}

TEST(BinaryDecoder, DecodeMap)
{
    uint8_t buffer[] = { 0xA3, 0x18, 0x1D, 0x70, 0x45, 0x39, 0x33, 0x41, 0x31, 0x34, 0x32, 0x42, 0x32, 0x38, 0x32,
                         0x43, 0x34, 0x41, 0x44, 0x30, 0x19, 0x02, 0x8c, 0x10, 0x19, 0x60, 0x60, 0x83, 0xFA, 0x3F,
                         0x9D, 0x70, 0xA4, 0xFA, 0x40, 0x91, 0xEB, 0x85, 0xFA, 0x40, 0xFC, 0x7A, 0xE1 };
    FixedDepthThingSetBinaryDecoder decoder(buffer, sizeof(buffer));
    char nodeId[17];
    uint8_t canAddr;
    std::array<float, 3> three;
    ASSERT_TRUE(decoder.decodeMap<uint16_t>([&](uint16_t &key) {
        switch (key) {
            case 0x1D:
                return decoder.decode(nodeId);
            case 0x028C:
                return decoder.decode(&canAddr);
            case 0x6060:
                return decoder.decode(&three);
            default:
                return false;
        }
    }));
    ASSERT_STREQ("E93A142B282C4AD0", nodeId);
    ASSERT_EQ(0x10, canAddr);
    ASSERT_EQ(1.23f, three[0]);
}