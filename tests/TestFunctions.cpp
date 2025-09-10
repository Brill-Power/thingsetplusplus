/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <thingset++/ThingSet.hpp>
#include "Buffer.hpp"
#include "gtest/gtest.h"

using namespace ThingSet;

static bool called = false;
static bool calledVoid = false;

static void test(int argi, float argf)
{
    called = true;
}

static void voidvoid()
{
    calledVoid = true;
}

ThingSetUserFunction<0x400, 0x0, "xTest", void, int, float> xTestFunc(test);
ThingSetUserFunction<0x410, 0x0, "xVoid", void> xVoidVoidFunc(voidvoid);
ThingSetUserFunction<0x420, 0x0, "xLambda", float, float, float> xFloatLambda([](float x, float y) { return x * y; });
ThingSetUserFunction<0x430, 0x0, "xMap", std::map<std::string, float>> xGetMap([]() {
    auto map = std::map<std::string, float>();
    map.insert_or_assign("hello", 1.0f);
    map.insert_or_assign("world", 2.0f);
    return map;
});

TEST(Functions, FunctionTypes)
{
    ASSERT_EQ("(i32,f32)->()", xTestFunc.getType());
    ASSERT_EQ("()->()", xVoidVoidFunc.getType());
    ASSERT_EQ("(f32,f32)->(f32)", xFloatLambda.getType());
}

TEST(Functions, FunctionParameters)
{
    ThingSetNode *node;
    ASSERT_TRUE(ThingSetRegistry::findById(0x401, &node));
    ASSERT_EQ(0x400, node->getParentId());
    ASSERT_EQ("xTesti32_1", node->getName());
    ASSERT_TRUE(ThingSetRegistry::findById(0x402, &node));
    ASSERT_EQ(0x400, node->getParentId());
    ASSERT_EQ("xTestf32_2", node->getName());

    ASSERT_TRUE(ThingSetRegistry::findById(0x421, &node));
    ASSERT_EQ(0x420, node->getParentId());
    ASSERT_EQ("xLambdaf32_1", node->getName());
}

TEST(Functions, InvokeLambda)
{
    uint8_t request[] = { 0x82, 0xF9, 0x40, 0x00, 0xF9, 0x42, 0x00 };
    uint8_t response[64];
    FixedDepthThingSetBinaryDecoder decoder(request, sizeof(request));
    FixedDepthThingSetBinaryEncoder encoder(response, sizeof(response));
    ASSERT_TRUE(xFloatLambda.invoke(decoder, encoder));
    uint8_t expected[] = { 0xFA, 0x40, 0xC0, 0x00, 0x00 }; // 6.0
    ASSERT_EQ(0, memcmp(response, expected, sizeof(expected)));
}

TEST(Functions, InvokeGetMap)
{
    uint8_t request[] = { 0x80 };
    uint8_t response[64];
    FixedDepthThingSetBinaryDecoder decoder(request, sizeof(request));
    FixedDepthThingSetBinaryEncoder encoder(response, sizeof(response));
    ASSERT_TRUE(xGetMap.invoke(decoder, encoder));
    uint8_t expected[] = {
        0xA2,                               // 2 element map
        0x65, 0x68, 0x65, 0x6C, 0x6C, 0x6F, // hello
        0xFA, 0x3F, 0x80, 0x00, 0x00,       // 1.0
        0x65, 0x77, 0x6F, 0x72, 0x6C, 0x64, // world
        0xFA, 0x40, 0x00, 0x00, 0x00        // 2.0
    };
    ASSERT_EQ(0, memcmp(response, expected, sizeof(expected)));
}

static uint32_t getLength(Buffer<1024> &buffer)
{
    return buffer.size();
}

ThingSetUserFunction<0x440, 0x0, "xGetLength", uint32_t, Buffer<1024> &> xGetLength(getLength);

TEST(Functions, InvokeFunctionWithDecodableParameter)
{
    uint8_t request[] = { 0x81, 0x44, 0x00, 0x01, 0x02, 0x03 };
    uint8_t response[64];
    FixedDepthThingSetBinaryDecoder decoder(request, sizeof(request));
    FixedDepthThingSetBinaryEncoder encoder(response, sizeof(response));
    ASSERT_TRUE(xGetLength.invoke(decoder, encoder));
    uint8_t expected[] = { 0x04 }; // function returns length of bytes passed in
    ASSERT_EQ(0, memcmp(response, expected, sizeof(expected)));
}