/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <thingset++/ip/asio/ThingSetAsyncSocketServerTransport.hpp>
#include <thingset++/ip/asio/ThingSetAsyncSocketSubscriptionTransport.hpp>
#include <thingset++/ThingSetClient.hpp>
#include <thingset++/ThingSetServer.hpp>
#include <thingset++/ThingSetFunction.hpp>
#include <thingset++/ThingSetListener.hpp>
#include <gtest/gtest.h>
#include <asio.hpp>
#include <thread>

using namespace ThingSet;
using namespace ThingSet::Ip::Async;
using namespace asio;

static std::array<uint8_t, 1024> rxBuffer;
static std::array<uint8_t, 1024> txBuffer;

TEST(AsioIpPublishSubscribe, Name)
{
    ThingSetReadWriteProperty totalVoltage { 0x300, 0, "totalVoltage", 24.0f };

    io_context serverContext(1);
    ThingSetAsyncSocketServerTransport serverTransport(serverContext);
    auto server = ThingSetServerBuilder::build(serverTransport);
    server.listen();

    io_context clientContext(1);
    auto endpoint = asio::ip::tcp::endpoint(asio::ip::address_v4::loopback(), 9001);
    ThingSetAsyncSocketSubscriptionTransport subscriptionTransport(clientContext);
    auto listener = ThingSetListenerBuilder::build(subscriptionTransport);
    size_t receiveCount = 0;
    listener.subscribe([&](auto sender, auto id) {
        if (id == 0x300) {
            receiveCount++;
            if (receiveCount == 5) {
                clientContext.stop();
            }
        }
    });

    std::thread serverThread([&]()
    {
        for (int i = 0; i < 5; i++)
        {
            server.publish(totalVoltage);
            std::this_thread::sleep_for(std::chrono::milliseconds(25));
        }
    });

    std::thread clientThread([&]()
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(125));
        clientContext.run_for(chrono::seconds(5));
        serverContext.stop();
    });

    clientThread.join();
    serverThread.join();

    ASSERT_EQ(5, receiveCount);
}
