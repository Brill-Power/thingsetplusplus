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
