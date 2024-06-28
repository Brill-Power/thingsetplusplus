#include "ThingSetProperty.hpp"
#include "ThingSetRegistry.hpp"
#include "gtest/gtest.h"

using namespace ThingSet;

TEST(Properties, SimpleProperty)
{
    ThingSetProperty<0x100, "f32", float> f32;
    ThingSetProperty<0x200, "i32", int32_t> i32;
    ThingSetProperty<0x201, "u32", uint32_t> u32;

    ThingSetNode *node;
    ASSERT_TRUE(ThingSetRegistry::findById(0x100, &node));
    ASSERT_EQ(0x100, node->getId());
    ASSERT_FALSE(ThingSetRegistry::findById(0x101, &node));

    size_t count = 0;
    for (ThingSetNode *n : ThingSetRegistry::getInstance()) {
        count++;
    }
    ASSERT_EQ(3, count);

    f32 = 1.0;
    u32 = 1U;

    float &f = f32.getValue();

    uint8_t buffer[128];
    FixedSizeThingSetBinaryEncoder<128> encoder(buffer);
    f32.encode(encoder);

    ASSERT_EQ(5, encoder.getEncodedLength());
    ASSERT_EQ(0xFA, buffer[0]);
}