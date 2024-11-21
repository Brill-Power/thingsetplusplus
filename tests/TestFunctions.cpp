#include "thingset++/ThingSet.hpp"
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
ThingSetUserFunction<0x403, 0x0, "xVoid", void> xVoidVoidFunc(voidvoid);
ThingSetUserFunction<0x404, 0x0, "xLambda", float, float, float> xFloatLambda([](float x, float y) { return x * y; });

TEST(Functions, FunctionTypes)
{
    ASSERT_EQ("(i32,f32)->()", xTestFunc.getType());
    ASSERT_EQ("()->()", xVoidVoidFunc.getType());
    ASSERT_EQ("(f32,f32)->(f32)", xFloatLambda.getType());
}

TEST(Functions, InvokeLambda)
{
    uint8_t request[] = { 0x82, 0xF9, 0x40, 0x00, 0xF9, 0x42, 0x00 };
    uint8_t response[64];
    FixedSizeThingSetBinaryDecoder decoder(request, sizeof(request));
    FixedSizeThingSetBinaryEncoder encoder(response, sizeof(response));
    ASSERT_TRUE(xFloatLambda.invoke(decoder, encoder));
    uint8_t expected[] = { 0xFA, 0x40, 0xC0, 0x00, 0x00 }; // 6.0
    ASSERT_EQ(0, memcmp(response, expected, sizeof(expected)));
}