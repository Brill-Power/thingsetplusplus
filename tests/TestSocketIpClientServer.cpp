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

static ThingSetReadWriteProperty<0x300, 0, "totalVoltage", float> totalVoltage = 24;

static ThingSetUserFunction<0x1000, 0x0, "xAddNumber", int, int, int> doSomething([](auto x, auto y) { return x + y; });

#define SOCKET_TEST(Name, Body) \
TEST(SocketIpClientServer, Name) \
{ \
    ThingSetSocketServerTransport serverTransport; \
    auto server = ThingSetServerBuilder::build(serverTransport); \
    server.listen(); \
    std::thread serverThread([&]() \
    { \
        printf("Server waiting\n"); \
        std::this_thread::sleep_for(std::chrono::seconds(5)); \
        printf("Server shutdown\n"); \
    }); \
\
    auto endpoint = "127.0.0.1"; \
    ThingSetSocketClientTransport clientTransport(endpoint); \
    auto client = ThingSetClient(clientTransport, rxBuffer, txBuffer); \
    bool clientRanSuccessfully = false; \
    std::thread clientThread([&]() \
    { \
        std::this_thread::sleep_for(std::chrono::milliseconds(125)); \
        printf("Connecting..."); \
        ASSERT_TRUE(client.connect()); \
        printf("done\n"); \
        Body \
        clientRanSuccessfully = true; \
    }); \
\
    clientThread.join(); \
    serverThread.join(); \
\
    ASSERT_TRUE(clientRanSuccessfully); \
}

SOCKET_TEST(GetFloat,
    float tv;
    printf("Getting...");
    ASSERT_TRUE(client.get(0x300, tv));
    printf("done\n");
    ASSERT_EQ(24.0f, tv);
)

SOCKET_TEST(ExecFunction,
    int result;
    printf("Execing...");
    ASSERT_TRUE(client.exec(0x1000, &result, 2, 3));
    printf("done\n");
    ASSERT_EQ(5, result);
)

SOCKET_TEST(UpdateFloat,
    ASSERT_TRUE(client.update("totalVoltage", 25.0f));
    ASSERT_EQ(25.0, totalVoltage.getValue());
)