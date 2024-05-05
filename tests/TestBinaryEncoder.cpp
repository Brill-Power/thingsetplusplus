#include "ThingSetBinaryEncoder.hpp"
#include "gtest/gtest.h"

using namespace ThingSet;

#define SETUP(size)                                                                                                    \
    uint8_t buffer[size];                                                                                              \
    FixedSizeThingSetBinaryEncoder<size> encoder(buffer);

#define ASSERT_BUFFER_EQ(expected, actual, actual_len)                                                                 \
    ASSERT_EQ(sizeof(expected), actual_len);                                                                           \
    for (size_t i = 0; i < sizeof(expected); i++) {                                                                    \
        ASSERT_EQ(expected[i], actual[i]);                                                                             \
    }

TEST(BinaryEncoder, EncodeFloat)
{
    SETUP(128)
    float f = 1.23;
    encoder.encode(f);
    uint8_t expected[] = { 0xFA, 0x3F, 0x9D, 0x70, 0xA4 };
    ASSERT_BUFFER_EQ(expected, buffer, encoder.getEncodedLength());
}

TEST(BinaryEncoder, EncodeArrayOfFloats)
{
    SETUP(128)
    std::array f = { 1.23f, 4.56f, 7.89f };
    encoder.encode(f);
    uint8_t expected[] = { 0x83, 0xFA, 0x3F, 0x9D, 0x70, 0xA4, 0xFA, 0x40,
                           0x91, 0xEB, 0x85, 0xFA, 0x40, 0xFC, 0x7A, 0xE1 };
    ASSERT_BUFFER_EQ(expected, buffer, encoder.getEncodedLength());
}