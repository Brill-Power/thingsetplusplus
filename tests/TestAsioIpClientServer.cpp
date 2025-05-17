/*
 * Copyright (c) 2024 Brill Power.
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

std::array<uint8_t, 1024> rxBuffer;
std::array<uint8_t, 1024> txBuffer;

ThingSetReadWriteProperty<0x300, 0, "totalVoltage", float> totalVoltage = 24;

ThingSetUserFunction<0x1000, 0x0, "xAddNumber", int, int, int> doSomething([](auto x, auto y) { return x + y; });

#define ASIO_TEST(Name, Body) \
TEST(AsioIpClientServer, Name) \
{ \
    io_context serverContext(1); \
    ThingSetAsyncSocketServerTransport serverTransport(serverContext); \
    auto server = ThingSetServerBuilder::build(serverTransport); \
    server.listen(); \
    bool runServer = true; \
    std::thread serverThread([&]() \
    { \
        serverContext.run_for(chrono::seconds(5)); \
    }); \
\
    io_context clientContext(1); \
    auto endpoint = asio::ip::tcp::endpoint(asio::ip::address_v4::loopback(), 9001); \
    ThingSetAsyncSocketClientTransport clientTransport(clientContext, endpoint); \
    auto client = ThingSetClient(clientTransport, rxBuffer, txBuffer); \
    bool clientRan = false; \
    std::thread clientThread([&]() \
    { \
        std::this_thread::sleep_for(std::chrono::milliseconds(125)); \
        ASSERT_TRUE(client.connect()); \
        Body \
        clientRan = true; \
        serverContext.stop(); \
    }); \
\
    clientThread.join(); \
    serverThread.join(); \
\
    ASSERT_TRUE(clientRan); \
}

ASIO_TEST(GetFloat,
    float totalVoltage;
    ASSERT_TRUE(client.get(0x300, totalVoltage));
    ASSERT_EQ(24.0f, totalVoltage);
)

ASIO_TEST(ExecFunction,
    int result;
    ASSERT_TRUE(client.exec(0x1000, &result, 2, 3));
    ASSERT_EQ(5, result);
)

