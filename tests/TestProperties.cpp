/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "gtest/gtest.h"
#include <thingset++/ThingSet.hpp>

using namespace ThingSet;

TEST(Properties, SimpleProperty)
{
    ThingSetReadOnlyProperty<float> f32 { 0x100, 0, "f32" };
    ThingSetReadOnlyProperty<int32_t> i32 { 0x200, 0, "i32" };
    ThingSetReadOnlyProperty<uint32_t> u32 { 0x201, 0, "u32" };

    ThingSetNode *node;
    ASSERT_TRUE(ThingSetRegistry::findById(0x100, &node));
    ASSERT_EQ(0x100, node->getId());
    ASSERT_FALSE(ThingSetRegistry::findById(0x102, &node));

    f32 = 1.0;
    u32 = 1U;

    uint8_t buffer[128];
    FixedDepthThingSetBinaryEncoder encoder(buffer, sizeof(buffer));
    f32.encode(encoder);

    ASSERT_EQ(5, encoder.getEncodedLength());
    ASSERT_EQ(0xFA, buffer[0]);
}

TEST(Properties, PointerProperty)
{
    float rf32 = 1.0;
    ThingSetReadOnlyProperty prf32 { 0x101, 0, "rf32", &rf32 };
    ASSERT_EQ("f32", prf32.getType());
    prf32 = 1.5;
    ASSERT_EQ(1.5, rf32);

    ThingSetNode *node;
    ASSERT_TRUE(ThingSetRegistry::findById(0x101, &node));
    ASSERT_EQ(0x101, node->getId());

    uint8_t buffer[128];
    FixedDepthThingSetBinaryEncoder encoder(buffer, sizeof(buffer));
    prf32.encode(encoder);

    ASSERT_EQ(5, encoder.getEncodedLength());
    ASSERT_EQ(0xFA, buffer[0]);
}

TEST(Properties, ArrayProperty)
{
    ThingSetReadOnlyProperty<std::array<uint32_t, 4>> u32s { 0x301, 0, "u32s", { { 0, 1, 2, 3 } } };
    ASSERT_EQ("u32[]", u32s.getType());
    u32s[0] = 5;
    ASSERT_EQ(5, u32s[0]);

    ThingSetNode *node;
    ASSERT_TRUE(ThingSetRegistry::findById(0x301, &node));
    ASSERT_EQ(0x301, node->getId());

    uint8_t buffer[128];
    FixedDepthThingSetBinaryEncoder encoder(buffer, sizeof(buffer));
    u32s.encode(encoder);

    ASSERT_EQ(5, encoder.getEncodedLength());
    ASSERT_EQ(0x84, buffer[0]);
}