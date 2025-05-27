/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "thingset++/ip/asio/ThingSetAsyncSocketClientTransport.hpp"
#include "thingset++/ip/asio/ThingSetAsyncSocketServerTransport.hpp"
#include "thingset++/ThingSetClient.hpp"
#include "thingset++/ThingSetServer.hpp"
#include "thingset++/ThingSetFunction.hpp"
#include "gtest/gtest.h"
#include <asio.hpp>
#include <thread>

using namespace ThingSet;
using namespace ThingSet::Ip::Async;
using namespace asio;

static std::array<uint8_t, 1024> rxBuffer;
static std::array<uint8_t, 1024> txBuffer;

#define ASIO_TEST(Name, Body) \
TEST(AsioIpClientServer, Name) \
{ \
    ThingSetReadWriteProperty<0x300, 0, "totalVoltage", float> totalVoltage = 24; \
    ThingSetUserFunction<0x1000, 0x0, "xAddNumber", int, int, int> doSomething([](auto x, auto y) { return x + y; }); \
\
    io_context serverContext(1); \
    ThingSetAsyncSocketServerTransport serverTransport(serverContext); \
    auto server = ThingSetServerBuilder::build(serverTransport); \
    server.listen(); \
    std::thread serverThread([&]() \
    { \
        serverContext.run_for(chrono::seconds(5)); \
    }); \
\
    io_context clientContext(1); \
    auto endpoint = asio::ip::tcp::endpoint(asio::ip::address_v4::loopback(), 9001); \
    ThingSetAsyncSocketClientTransport clientTransport(clientContext, endpoint); \
    auto client = ThingSetClient(clientTransport, rxBuffer, txBuffer); \
    bool clientRanSuccessfully = false; \
    std::thread clientThread([&]() \
    { \
        std::this_thread::sleep_for(std::chrono::milliseconds(125)); \
        ASSERT_TRUE(client.connect()); \
        Body \
        clientRanSuccessfully = true; \
        serverContext.stop(); \
    }); \
\
    clientThread.join(); \
    serverThread.join(); \
\
    ASSERT_TRUE(clientRanSuccessfully); \
}

ASIO_TEST(GetFloat,
    float tv;
    ASSERT_TRUE(client.get(0x300, tv));
    ASSERT_EQ(24.0f, tv);
)

ASIO_TEST(ExecFunction,
    int result;
    ASSERT_TRUE(client.exec(0x1000, &result, 2, 3));
    ASSERT_EQ(5, result);
)

ASIO_TEST(UpdateFloat,
    ASSERT_TRUE(client.update("totalVoltage", 25.0f));
    ASSERT_EQ(25.0, totalVoltage.getValue());
)