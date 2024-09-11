#include "ThingSet.hpp"
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

TEST(Functions, FunctionTypes)
{
    ASSERT_EQ("(i32,f32)->()", xTestFunc.getType());
    ASSERT_EQ("()->()", xVoidVoidFunc.getType());
}