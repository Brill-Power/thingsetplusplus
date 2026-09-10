/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "gtest/gtest.h"
#include <thingset++/ThingSet.hpp>
#include <thingset++/ThingSetServer.hpp>

using namespace ThingSet;

namespace {

class TestableServer : public _ThingSetServer
{
public:
    TestableServer() : _ThingSetServer(nullptr)
    {}

    bool listen() override
    {
        return true;
    }

    using _ThingSetServer::handleBinaryRequest;
};

/// @brief Builds a binary GET request for the given node ID.
static size_t buildGetRequest(uint16_t id, uint8_t *request, size_t requestSize)
{
    request[0] = (uint8_t)ThingSetBinaryRequestType::get;
    FixedDepthThingSetBinaryEncoder encoder(request + 1, requestSize - 1);
    EXPECT_TRUE(encoder.encode(id));
    return 1 + encoder.getEncodedLength();
}

} // namespace

TEST(Server, GetGroupThatFitsReturnsContent)
{
    ThingSetGroup<0x700, 0, "TestFitGroup"> group;
    ThingSetReadOnlyProperty<float> f32 { 0x701, 0x700, "f32", 1.5f };

    TestableServer server;
    uint8_t request[8];
    size_t requestLen = buildGetRequest(0x700, request, sizeof(request));
    uint8_t response[256];
    int responseLen = server.handleBinaryRequest(request, requestLen, response, sizeof(response));

    ASSERT_GT(responseLen, 1);
    ASSERT_EQ(ThingSetStatusCode::content, response[0]);
}

TEST(Server, GetGroupTooBigForResponseBufferReturnsRequestTooLarge)
{
    ThingSetGroup<0x710, 0, "TestBigGroup"> group;
    std::array<float, 176> table;
    table.fill(1.0f);
    ThingSetReadOnlyReferenceProperty<std::array<float, 176>> bigArray { 0x711, 0x710, "bigArray",
                                                                         table };

    TestableServer server;
    uint8_t request[8];
    size_t requestLen = buildGetRequest(0x710, request, sizeof(request));
    // response buffer far too small for ~880 bytes of encoded floats
    uint8_t response[128];
    int responseLen = server.handleBinaryRequest(request, requestLen, response, sizeof(response));

    // only the status byte; no truncated payload
    ASSERT_EQ(1, responseLen);
    ASSERT_EQ(ThingSetStatusCode::requestTooLarge, response[0]);
}

TEST(Server, GetPropertyTooBigForResponseBufferReturnsRequestTooLarge)
{
    std::array<float, 176> table;
    table.fill(1.0f);
    ThingSetReadOnlyReferenceProperty<std::array<float, 176>> bigArray { 0x721, 0, "bigArrayTopLevel",
                                                                         table };

    TestableServer server;
    uint8_t request[8];
    size_t requestLen = buildGetRequest(0x721, request, sizeof(request));
    uint8_t response[128];
    int responseLen = server.handleBinaryRequest(request, requestLen, response, sizeof(response));

    ASSERT_EQ(1, responseLen);
    ASSERT_EQ(ThingSetStatusCode::requestTooLarge, response[0]);
}
