/*
 * Copyright (c) 2026 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "thingset++/ThingSetClient.hpp"
#include <algorithm>
#include <array>
#include <cerrno>
#include <cstring>
#include <gtest/gtest.h>
#include <vector>

using namespace ThingSet;

namespace {

/// Scriptable in-memory transport for exercising ThingSetClient error paths.
class FakeClientTransport : public ThingSetClientTransport
{
public:
    bool connectResult = true;
    bool writeResult = true;
    /// Value returned by read(); when positive, up to that many bytes of
    /// `response` are copied into the caller's buffer first.
    int readResult = 0;
    std::vector<uint8_t> response;

    bool connect() override
    {
        return connectResult;
    }

    int read(uint8_t *buffer, size_t len) override
    {
        if (readResult > 0 && !response.empty()) {
            size_t count = std::min({ (size_t)readResult, response.size(), len });
            memcpy(buffer, response.data(), count);
        }
        return readResult;
    }

    bool write(uint8_t *, size_t) override
    {
        return writeResult;
    }
};

struct ClientFixture
{
    FakeClientTransport transport;
    std::array<uint8_t, 64> rxBuffer;
    std::array<uint8_t, 64> txBuffer;
    ThingSetClient client;

    ClientFixture() : client(transport, rxBuffer, txBuffer)
    {}
};

} // namespace

TEST(Client, ExecSucceedsOnWellFormedResponse)
{
    ClientFixture f;
    f.transport.response = { (uint8_t)ThingSetStatusCode::changed, 0xF6, 0x00 };
    f.transport.readResult = 3;

    int ret = -1;
    ThingSetResult result = f.client.exec(0x1234, &ret);
    EXPECT_TRUE(result.success());
    EXPECT_EQ(ret, 0);
}

/// Regression test: a receive timeout after a previous successful exchange
/// must fail, even though the stale success response is still sitting in the
/// client's rx buffer. The negative errno from the transport used to be
/// assigned to a size_t, defeating the empty-response check and decoding the
/// stale bytes as a fresh (successful) response.
TEST(Client, ExecFailsOnReadTimeoutDespiteStaleBuffer)
{
    ClientFixture f;
    f.transport.response = { (uint8_t)ThingSetStatusCode::changed, 0xF6, 0x00 };
    f.transport.readResult = 3;

    int ret = -1;
    ASSERT_TRUE(f.client.exec(0x1234, &ret).success());

    // the device stops answering: msgq timeout surfaces as -EAGAIN
    f.transport.readResult = -EAGAIN;

    ThingSetResult result = f.client.exec(0x1234, &ret);
    EXPECT_FALSE(result.success());
    EXPECT_EQ(result.code(), ThingSetStatusCode::gatewayTimeout);
}

TEST(Client, ExecFailsOnZeroLengthRead)
{
    ClientFixture f;
    f.transport.readResult = 0;

    int ret = -1;
    ThingSetResult result = f.client.exec(0x1234, &ret);
    EXPECT_FALSE(result.success());
    EXPECT_EQ(result.code(), ThingSetStatusCode::gatewayTimeout);
}

/// A success status byte with no CBOR null after it must not be decoded.
TEST(Client, ExecFailsOnTruncatedResponse)
{
    ClientFixture f;
    f.transport.response = { (uint8_t)ThingSetStatusCode::changed };
    f.transport.readResult = 1;

    int ret = -1;
    ThingSetResult result = f.client.exec(0x1234, &ret);
    EXPECT_FALSE(result.success());
    EXPECT_EQ(result.code(), ThingSetStatusCode::internalServerError);
}

/// A single-byte error response is legitimate and must surface the device's
/// own status code (distinguishable from a timeout).
TEST(Client, ExecReturnsDeviceErrorStatus)
{
    ClientFixture f;
    f.transport.response = { (uint8_t)ThingSetStatusCode::badRequest };
    f.transport.readResult = 1;

    int ret = -1;
    ThingSetResult result = f.client.exec(0x1234, &ret);
    EXPECT_FALSE(result.success());
    EXPECT_EQ(result.code(), ThingSetStatusCode::badRequest);
}
