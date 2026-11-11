/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "gtest/gtest.h"
#include <thingset++/ThingSet.hpp>
#include <thingset++/ThingSetServer.hpp>
#include <string>
#include <vector>

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
    using _ThingSetServer::handleTextRequest;
};

struct TestRecord
{
    ThingSetReadWriteRecordMember<0x7F1, 0x7F0, "x", int32_t> x;
};

/// @brief Sends a binary request given as hex bytes and returns the response length; the
/// response is left in `response`.
static int sendBinary(TestableServer &server, const char *hex, uint8_t *response, size_t responseSize)
{
    std::vector<uint8_t> request;
    std::string digits;
    for (const char *c = hex; *c; c++) {
        if (*c != ' ') {
            digits += *c;
        }
    }
    for (size_t i = 0; i + 1 < digits.size(); i += 2) {
        request.push_back((uint8_t)std::stoul(digits.substr(i, 2), nullptr, 16));
    }
    return server.handleBinaryRequest(request.data(), request.size(), response, responseSize);
}

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

// Updating a record array as a whole (rather than one element of it) is not
// supported. It must be refused without touching the array: value[SIZE_MAX] is
// out of bounds, and decoding into it corrupts whatever precedes the array.

TEST(Server, UpdateRecordArrayByIdIsBadRequest)
{
    ThingSetReadWriteProperty<std::array<TestRecord, 2>> records { 0x7F0, 0, "serverTestRecords" };
    records.getValue()[0].x = 1;
    records.getValue()[1].x = 2;

    TestableServer server;
    uint8_t response[64];
    // =0x7F0 {0x7F1: 9}
    int responseLen = sendBinary(server, "07 19 07F0 A1 19 07F1 09", response, sizeof(response));

    ASSERT_EQ(1, responseLen);
    ASSERT_EQ(ThingSetStatusCode::badRequest, response[0]);
    ASSERT_EQ(1, records.getValue()[0].x.getValue());
    ASSERT_EQ(2, records.getValue()[1].x.getValue());
}

TEST(Server, UpdateRecordArrayByNameIsBadRequest)
{
    ThingSetReadWriteProperty<std::array<TestRecord, 2>> records { 0x7F0, 0, "serverTestRecords" };
    records.getValue()[0].x = 1;
    records.getValue()[1].x = 2;

    TestableServer server;
    std::string request = "=serverTestRecords {\"x\":9}";
    uint8_t response[64];
    int responseLen = server.handleTextRequest((uint8_t *)request.data(), request.size(), response, sizeof(response));

    ASSERT_GE(responseLen, 3);
    ASSERT_EQ(":A0", std::string((char *)response, 3));
    ASSERT_EQ(1, records.getValue()[0].x.getValue());
    ASSERT_EQ(2, records.getValue()[1].x.getValue());
}

TEST(Server, UpdateRecordArrayElementByNameIsChanged)
{
    ThingSetReadWriteProperty<std::array<TestRecord, 2>> records { 0x7F0, 0, "serverTestRecords" };
    records.getValue()[0].x = 1;
    records.getValue()[1].x = 2;

    TestableServer server;
    std::string request = "=serverTestRecords/1 {\"x\":9}";
    uint8_t response[64];
    int responseLen = server.handleTextRequest((uint8_t *)request.data(), request.size(), response, sizeof(response));

    ASSERT_GE(responseLen, 3);
    ASSERT_EQ(":84", std::string((char *)response, 3));
    ASSERT_EQ(1, records.getValue()[0].x.getValue());
    ASSERT_EQ(9, records.getValue()[1].x.getValue());
}

TEST(Server, UpdateRecordArrayElementWithBadValueIsBadRequest)
{
    ThingSetReadWriteProperty<std::array<TestRecord, 2>> records { 0x7F0, 0, "serverTestRecords" };
    records.getValue()[0].x = 1;
    records.getValue()[1].x = 2;

    TestableServer server;
    uint8_t response[64];
    for (const char *bad : { "=serverTestRecords/1 {\"x\":\"nine\"}", /* wrong type */
                             "=serverTestRecords/1 {\"x\":9999999999}" /* out of range */ }) {
        std::string request = bad;
        int responseLen =
            server.handleTextRequest((uint8_t *)request.data(), request.size(), response, sizeof(response));

        ASSERT_GE(responseLen, 3) << request;
        ASSERT_EQ(":A0", std::string((char *)response, 3)) << request;
        ASSERT_EQ(1, records.getValue()[0].x.getValue()) << request;
        ASSERT_EQ(2, records.getValue()[1].x.getValue()) << request;
    }

    // a truncated map is also a bad request, but members decoded before the
    // truncation keep their new values: updates are not transactional, as for
    // ordinary properties (see DISABLED_BinaryUpdateIsAllOrNothing)
    std::string truncated = "=serverTestRecords/1 {\"x\":9";
    int responseLen =
        server.handleTextRequest((uint8_t *)truncated.data(), truncated.size(), response, sizeof(response));
    ASSERT_GE(responseLen, 3);
    ASSERT_EQ(":A0", std::string((char *)response, 3));
}
