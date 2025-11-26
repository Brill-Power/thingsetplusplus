/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "gtest/gtest.h"
#include <thingset++/ThingSet.hpp>
#include "Buffer.hpp"

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

TEST(Properties, ReferenceProperty)
{
    float rf32 = 1.0;
    ThingSetReadOnlyProperty<float &> prf32 { 0x101, 0, "rf32", rf32 };
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

TEST(Properties, ConstPointerProperty)
{
    const float rf32 = 1.0;
    ThingSetReadOnlyProperty prf32 { 0x101, 0, "rf32", &rf32 };
    ASSERT_EQ("f32", prf32.getType());
    // won't compile as pointer is const
    // prf32 = 1.5;
    // ASSERT_EQ(1.5, rf32);

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


TEST(Properties, ReferenceToArrayProperty)
{
    std::array<uint32_t, 4> u32s { 0, 1, 2, 3};
    ThingSetReadOnlyProperty<std::array<uint32_t, 4> &> pu32s { 0x301, 0, "u32s", u32s };
    ASSERT_EQ("u32[]", pu32s.getType());
    u32s[0] = 5;
    ASSERT_EQ(5, u32s[0]);

    ThingSetNode *node;
    ASSERT_TRUE(ThingSetRegistry::findById(0x301, &node));
    ASSERT_EQ(0x301, node->getId());

    uint8_t buffer[128];
    FixedDepthThingSetBinaryEncoder encoder(buffer, sizeof(buffer));
    pu32s.encode(encoder);

    ASSERT_EQ(5, encoder.getEncodedLength());
    ASSERT_EQ(0x84, buffer[0]);
}

TEST(Properties, InlineCArrayProperty)
{
    ThingSetReadOnlyProperty u32s { 0x302, 0, "u32s", (uint32_t[]){ 0, 1, 2, 3 } };
    ASSERT_EQ("u32[]", u32s.getType());
    u32s[0] = 5;
    ASSERT_EQ(5, u32s[0]);

    ThingSetNode *node;
    ASSERT_TRUE(ThingSetRegistry::findById(0x302, &node));
    ASSERT_EQ(0x302, node->getId());

    uint8_t buffer[128];
    FixedDepthThingSetBinaryEncoder encoder(buffer, sizeof(buffer));
    u32s.encode(encoder);

    ASSERT_EQ(5, encoder.getEncodedLength());
    ASSERT_EQ(0x84, buffer[0]);
}

TEST(Properties, CustomProperty)
{
    ThingSetReadOnlyProperty<Buffer<16>> someBuffer { 0x1001, 0, "someBuffer", { } };
    auto &b = someBuffer.getValue();
    b.buffer()[0] = 0x01;
    b.buffer()[1] = 0x02;
    b.buffer()[2] = 0x03;
    b.buffer()[3] = 0x04;
    b.size() = 4;

    ThingSetNode *node;
    ASSERT_TRUE(ThingSetRegistry::findById(0x1001, &node));
    ASSERT_EQ(0x1001, node->getId());

    uint8_t buffer[128];
    FixedDepthThingSetBinaryEncoder encoder(buffer, sizeof(buffer));
    someBuffer.encode(encoder);

    ASSERT_EQ(5, encoder.getEncodedLength());
    ASSERT_EQ(0x44, buffer[0]);
    ASSERT_EQ(0x02, buffer[2]);
}

TEST(Properties, StringConstantProperty)
{
    const char* str = "Unchanging";
    ThingSetReadOnlyProperty strop { 0x1002, 0, "str", str };
    ASSERT_EQ("string", strop.getType());

    ThingSetNode *node;
    ASSERT_TRUE(ThingSetRegistry::findById(0x1002, &node));
    ASSERT_EQ(0x1002, node->getId());

    uint8_t buffer[128];
    FixedDepthThingSetBinaryEncoder encoder(buffer, sizeof(buffer));
    strop.encode(encoder);

    ASSERT_EQ(11, encoder.getEncodedLength());
    ASSERT_EQ(0x6A, buffer[0]);
}

TEST(Properties, InlineStringConstantProperty)
{
    ThingSetReadOnlyProperty strop { 0x1003, 0, "str", "Unchanging" };
    ASSERT_EQ("string", strop.getType());

    ThingSetNode *node;
    ASSERT_TRUE(ThingSetRegistry::findById(0x1003, &node));
    ASSERT_EQ(0x1003, node->getId());

    uint8_t buffer[128];
    FixedDepthThingSetBinaryEncoder encoder(buffer, sizeof(buffer));
    strop.encode(encoder);

    ASSERT_EQ(11, encoder.getEncodedLength());
    ASSERT_EQ(0x6A, buffer[0]);
}