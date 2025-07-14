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
    ThingSetProperty<float> f32 { 0x100, 0, "f32", ThingSetAccess::anyRead };
    ThingSetProperty<int32_t> i32 { 0x200, 0, "i32", ThingSetAccess::anyRead };
    ThingSetProperty<uint32_t> u32 { 0x201, 0, "u32", ThingSetAccess::anyRead };

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
    ThingSetProperty prf32 { 0x101, 0, "rf32", ThingSetAccess::anyRead, &rf32 };
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