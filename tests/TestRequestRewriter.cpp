/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "thingset++/ThingSetRequestContext.hpp"
#include "thingset++/ThingSetBinaryEncoder.hpp"
#include "gtest/gtest.h"

using namespace ThingSet;

#define SETUP(size, verb)                                                                                              \
    uint8_t buffer[size];                                                                                              \
    buffer[0] = (uint8_t)verb;                                                                                         \
    FixedDepthThingSetBinaryEncoder encoder(buffer + 1, size - 1);

TEST(RequestRewriter, BasicBinary)
{
    SETUP(128, ThingSetBinaryRequestType::get);
    encoder.encode("/deadbeef12345678/Something");
    ThingSetBinaryRequestContext context(buffer, 128, {}, 0);
    std::string nodeId;
    uint8_t *newRequest = &buffer[0];
    size_t newLength = context.rewrite(&newRequest, encoder.getEncodedLength(), nodeId);
    ASSERT_EQ((uint8_t)ThingSetBinaryRequestType::get, newRequest[0]);
    ASSERT_EQ("deadbeef12345678", nodeId);
    ASSERT_EQ(11, newLength);
}

TEST(RequestRewriter, NewBinary)
{
    SETUP(128, ThingSetBinaryRequestType::forward);
    encoder.encode("deadbeef12345678");
    buffer[encoder.getEncodedLength() + 1] = (uint8_t)ThingSetBinaryRequestType::get;
    ThingSetBinaryRequestContext context(buffer, 128, {}, 0);
    std::string nodeId;
    uint8_t *newRequest = &buffer[0];
    size_t newLength = context.rewrite(&newRequest, encoder.getEncodedLength() + 2, nodeId);
    ASSERT_EQ((uint8_t)ThingSetBinaryRequestType::get, newRequest[0]);
    ASSERT_EQ("deadbeef12345678", nodeId);
    ASSERT_EQ(1, newLength);
}

TEST(RequestRewriter, NewText)
{
    SETUP(128, ThingSetTextRequestType::forward);
    memcpy(&buffer[1], "deadbeef12345678", 16);
    buffer[17] = (uint8_t)ThingSetTextRequestType::get;
    ThingSetTextRequestContext context(buffer, 128, {}, 0);
    std::string nodeId;
    uint8_t *newRequest = &buffer[0];
    size_t newLength = context.rewrite(&newRequest, 18, nodeId);
    ASSERT_EQ((uint8_t)ThingSetTextRequestType::get, newRequest[0]);
    ASSERT_EQ("deadbeef12345678", nodeId);
    ASSERT_EQ(1, newLength);
}
