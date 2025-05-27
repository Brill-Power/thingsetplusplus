/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "thingset++/ip/sockets/ThingSetSocketClientTransport.hpp"
#include "thingset++/ip/sockets/ThingSetSocketServerTransport.hpp"
#include "thingset++/ThingSetClient.hpp"
#include "thingset++/ThingSetServer.hpp"
#include "thingset++/ThingSetFunction.hpp"
#include "gtest/gtest.h"
#include <thread>

using namespace ThingSet;
using namespace ThingSet::Ip::Sockets;

static std::array<uint8_t, 1024> rxBuffer;
static std::array<uint8_t, 1024> txBuffer;

#define SOCKET_TEST(Name, Body) \
TEST(SocketIpClientServer, Name) \
{ \
    ThingSetReadWriteProperty<0x300, 0, "totalVoltage", float> totalVoltage = 24; \
    ThingSetUserFunction<0x1000, 0x0, "xAddNumber", int, int, int> doSomething([](auto x, auto y) { return x + y; }); \
\
    ThingSetSocketServerTransport serverTransport; \
    auto server = ThingSetServerBuilder::build(serverTransport); \
    server.listen(); \
\
    auto endpoint = "127.0.0.1"; \
    ThingSetSocketClientTransport clientTransport(endpoint); \
    auto client = ThingSetClient(clientTransport, rxBuffer, txBuffer); \
    bool clientRanSuccessfully = false; \
    std::thread clientThread([&]() \
    { \
        std::this_thread::sleep_for(std::chrono::milliseconds(125)); \
        ASSERT_TRUE(client.connect()); \
        Body \
        clientRanSuccessfully = true; \
    }); \
\
    clientThread.join(); \
\
    ASSERT_TRUE(clientRanSuccessfully); \
}

SOCKET_TEST(GetFloat,
    float tv;
    ASSERT_TRUE(client.get(0x300, tv));
    ASSERT_EQ(24.0f, tv);
)

SOCKET_TEST(ExecFunction,
    int result;
    ASSERT_TRUE(client.exec(0x1000, &result, 2, 3));
    ASSERT_EQ(5, result);
)

SOCKET_TEST(UpdateFloat,
    ASSERT_TRUE(client.update("totalVoltage", 25.0f));
    ASSERT_EQ(25.0, totalVoltage.getValue());
)