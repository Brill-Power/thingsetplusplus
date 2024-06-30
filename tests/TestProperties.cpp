#include "gtest/gtest.h"
#include <ThingSet.hpp>

using namespace ThingSet;

TEST(Properties, SimpleProperty)
{
    ThingSetProperty<0x100, 0, "f32", float> f32;
    ThingSetProperty<0x200, 0, "i32", int32_t> i32;
    ThingSetProperty<0x201, 0, "u32", uint32_t> u32;

    ThingSetNode *node;
    ASSERT_TRUE(ThingSetRegistry::findById(0x100, &node));
    ASSERT_EQ(0x100, node->getId());
    ASSERT_FALSE(ThingSetRegistry::findById(0x101, &node));

    f32 = 1.0;
    u32 = 1U;

    float &f = f32.getValue();

    uint8_t buffer[128];
    FixedSizeThingSetBinaryEncoder encoder(buffer, sizeof(buffer));
    f32.encode(encoder);

    ASSERT_EQ(5, encoder.getEncodedLength());
    ASSERT_EQ(0xFA, buffer[0]);
}