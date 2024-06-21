#include "ThingSetProperty.hpp"
#include "ThingSetRegistry.hpp"
#include "gtest/gtest.h"

using namespace ThingSet;

TEST(Properties, SimpleProperty)
{
    ThingSetProperty<0x100, float> f32("f32", 1.0f);
    ThingSetProperty<0x200, int32_t> i32("i32", 1);
    ThingSetProperty<0x201, uint32_t> u32("u32", 1U);

    ThingSetNode *node;
    ASSERT_TRUE(ThingSetRegistry::findById(0x100, &node));
    ASSERT_EQ(0x100, node->getId());
    ASSERT_FALSE(ThingSetRegistry::findById(0x101, &node));

    size_t count = 0;
    for (ThingSetNode *n : ThingSetRegistry::getInstance()) {
        count++;
    }
    ASSERT_EQ(3, count);
}