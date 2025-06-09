/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "thingset++/ThingSetTextDecoder.hpp"
#include "gtest/gtest.h"

using namespace ThingSet;

TEST(TextDecoder, DecodeStdString)
{
    char buffer[] = "\"Hello World\"";
    ThingSetTextDecoder decoder(buffer, strlen(buffer));
    std::string value;
    std::string *valuePtr = &value;
    bool ret = decoder.decode(valuePtr);
    ASSERT_TRUE(ret);
    ASSERT_EQ(value, "Hello World");
}

TEST(TextDecoder, DecodeChar)
{
    char buffer[] = "F";
    ThingSetTextDecoder decoder(buffer, strlen(buffer));
    char value;
    char *valuePtr = &value;
    bool ret = decoder.decode(valuePtr);
    ASSERT_TRUE(ret);
    ASSERT_EQ(value, 'F');
}

TEST(TextDecoder, DecodeDouble)
{
    char buffer[] = "12.4";
    ThingSetTextDecoder decoder(buffer, strlen(buffer));
    double value;
    double *valuePtr = &value;
    bool ret = decoder.decode(valuePtr);
    ASSERT_TRUE(ret);
    ASSERT_NEAR(*valuePtr, 12.4F, 1e-6);
}

TEST(TextDecoder, DecodeFloat)
{
    char buffer[] = "12.4";
    ThingSetTextDecoder decoder(buffer, strlen(buffer));
    float value;
    float *valuePtr = &value;
    bool ret = decoder.decode(valuePtr);
    ASSERT_TRUE(ret);
    ASSERT_NEAR(*valuePtr, 12.4F, 1e-6);
}

TEST(TextDecoder, DecodeUint8)
{
    char buffer[] = "255";
    ThingSetTextDecoder decoder(buffer, strlen(buffer));
    uint8_t value;
    uint8_t *valuePtr = &value;
    bool ret = decoder.decode(valuePtr);
    ASSERT_TRUE(ret);
    ASSERT_EQ(value, UINT8_MAX);
}

TEST(TextDecoder, DecodeUint16)
{
    char buffer[] = "65535";
    ThingSetTextDecoder decoder(buffer, strlen(buffer));
    uint16_t value;
    uint16_t *valuePtr = &value;
    bool ret = decoder.decode(valuePtr);
    ASSERT_TRUE(ret);
    ASSERT_EQ(value, UINT16_MAX);
}

TEST(TextDecoder, DecodeUint32)
{
    char buffer[] = "4294967295";
    ThingSetTextDecoder decoder(buffer, strlen(buffer));
    uint32_t value;
    uint32_t *valuePtr = &value;
    bool ret = decoder.decode(valuePtr);
    ASSERT_TRUE(ret);
    ASSERT_EQ(value, UINT32_MAX);
}

TEST(TextDecoder, DecodeUint64)
{
    char buffer[] = "18446744073709551615";
    ThingSetTextDecoder decoder(buffer, strlen(buffer));
    uint64_t value;
    uint64_t *valuePtr = &value;
    bool ret = decoder.decode(valuePtr);
    ASSERT_TRUE(ret);
    ASSERT_EQ(value, UINT64_MAX);
}

TEST(TextDecoder, DecodeInt8)
{
    char buffer[] = "-128";
    ThingSetTextDecoder decoder(buffer, strlen(buffer));
    int8_t value;
    int8_t *valuePtr = &value;
    bool ret = decoder.decode(valuePtr);
    ASSERT_TRUE(ret);
    ASSERT_EQ(value, INT8_MIN);
}

TEST(TextDecoder, DecodeInt16)
{
    char buffer[] = "-32768";
    ThingSetTextDecoder decoder(buffer, strlen(buffer));
    int16_t value;
    int16_t *valuePtr = &value;
    bool ret = decoder.decode(valuePtr);
    ASSERT_TRUE(ret);
    ASSERT_EQ(value, INT16_MIN);
}

TEST(TextDecoder, DecodeInt32)
{
    char buffer[] = "-2147483648";
    ThingSetTextDecoder decoder(buffer, strlen(buffer));
    int32_t value;
    int32_t *valuePtr = &value;
    bool ret = decoder.decode(valuePtr);
    ASSERT_TRUE(ret);
    ASSERT_EQ(value, INT32_MIN);
}

TEST(TextDecoder, DecodeInt64)
{
    char buffer[] = "-9223372036854775808";
    ThingSetTextDecoder decoder(buffer, strlen(buffer));
    uint64_t value;
    uint64_t *valuePtr = &value;
    bool ret = decoder.decode(valuePtr);
    ASSERT_TRUE(ret);
    ASSERT_EQ(value, INT64_MIN);
}

TEST(TextDecoder, DecodeList)
{
    char buffer[] = "[1234567890,9876543210]";
    ThingSetTextDecoder decoder(buffer, strlen(buffer));
    std::tuple<uint64_t, uint64_t> values;
    bool ret = decoder.decodeList(values);
    ASSERT_TRUE(ret);
    ASSERT_EQ(std::get<0>(values), 1234567890);
    ASSERT_EQ(std::get<1>(values), 9876543210);
}

TEST(TextDecoder, DecodeArray)
{
    char buffer[] = "[1.23,13.4,14.5]";
    ThingSetTextDecoder decoder(buffer, strlen(buffer));
    std::array<float, 3> values;
    bool ret = decoder.decode(&values);
    ASSERT_TRUE(ret);
    ASSERT_NEAR(values[0], 1.23F, 1e-6);
    ASSERT_NEAR(values[1], 13.4F, 1e-6);
    ASSERT_NEAR(values[2], 14.5F, 1e-6);
}

TEST(TextDecoder, DecodeMap)
{
    char buffer[] = "{\"nodeId\":\"E93A142B282C4AD0\",\"three\":[1.23,13.4,14.5],\"canAddr\":16}";
    ThingSetTextDecoder decoder(buffer, strlen(buffer));
    std::string nodeId;
    std::array<float, 3> three;
    uint8_t canAddr;
    ASSERT_TRUE(decoder.decodeMap<std::string>([&](std::string &key) {
        if (key == "nodeId") {
            return decoder.decode(&nodeId);
        }
        else if (key == "canAddr") {
            return decoder.decode(&canAddr);
        }
        else if (key == "three") {
            return decoder.decode(&three);
        }
        else {
            return false;
        }
    }));
    ASSERT_EQ("E93A142B282C4AD0", nodeId);
    ASSERT_NEAR(1.23f, three[0], 1e-6);
    ASSERT_NEAR(13.4f, three[1], 1e-6);
    ASSERT_NEAR(14.5f, three[2], 1e-6);
    ASSERT_EQ(0x10, canAddr);
}