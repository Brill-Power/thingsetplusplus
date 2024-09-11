#include "ThingSet.hpp"
#include "ThingSetCompatibility.hpp"
#include "gtest/gtest.h"

using namespace ThingSet;

TEST(Compatibility, SimpleProperty)
{
    float f;
    THINGSET_ADD_ITEM_FLOAT(0x0, 0x100, "xFloat", &f, THINGSET_ANY_R | THINGSET_MFR_W, 0)
}