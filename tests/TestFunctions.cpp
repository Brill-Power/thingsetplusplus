#include "gtest/gtest.h"
#include <ThingSet.hpp>

using namespace ThingSet;

static bool called = false;

static void test(int argi, float argf)
{
    called = true;
}

ThingSetFunction<0x400, 0x0, "xTest", void, int, float> xTestFunc(test);
ThingSetParameter<0x401, 0x400, "xTestInt", int> intArg;
ThingSetParameter<0x402, 0x400, "xTestFloat", float> floatArg;

TEST(Functions, SimpleFunction)
{}