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
#include <map>
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

TEST(AsioIpPublishSubscribe, Subset)
{
    // Four flavours of property: tagged live, tagged live|persisted (union),
    // tagged persisted-only, and untagged. publish(Subset::live) must include
    // the first two and exclude the rest.
    ThingSetReadWriteProperty<float,    Subset::live | Subset::persisted> bothSubsets   { 0x900, 0, "bothSubsets", 1.5f };
    ThingSetReadWriteProperty<uint32_t, Subset::live>                     liveOnly      { 0x901, 0, "liveOnly", 7u };
    ThingSetReadWriteProperty<uint32_t, Subset::persisted>                persistedOnly { 0x902, 0, "persistedOnly", 99u };
    ThingSetReadWriteProperty<uint32_t>                                   untagged      { 0x903, 0, "untagged", 42u };

    io_context serverContext(1);
    ThingSetAsyncSocketServerTransport serverTransport(serverContext);
    auto server = ThingSetServerBuilder::build(serverTransport);
    server.listen();

    io_context clientContext(1);
    ThingSetAsyncSocketSubscriptionTransport subscriptionTransport(clientContext);
    auto listener = ThingSetListenerBuilder::build(subscriptionTransport);

    std::map<uint16_t, size_t> hits;
    listener.subscribe([&](auto, auto id) {
        hits[id]++;
        // Stop once the last id from the third publish has arrived
        if (hits[0x901] == 3) {
            clientContext.stop();
        }
    });

    std::thread serverThread([&]() {
        for (int i = 0; i < 3; i++) {
            server.publish(Subset::live);
            std::this_thread::sleep_for(std::chrono::milliseconds(25));
        }
    });
    std::thread clientThread([&]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(125));
        clientContext.run_for(std::chrono::seconds(5));
        serverContext.stop();
    });
    clientThread.join();
    serverThread.join();

    EXPECT_EQ(3u, hits[0x900]);  // live | persisted matches live filter (union)
    EXPECT_EQ(3u, hits[0x901]);  // live matches
    EXPECT_EQ(0u, hits[0x902]);  // persisted-only excluded
    EXPECT_EQ(0u, hits[0x903]);  // untagged excluded
}

TEST(AsioIpPublishSubscribe, SubsetEmpty)
{
    // No properties tagged Subset::live in this scope, so publish must still
    // succeed and produce an empty map.
    io_context serverContext(1);
    ThingSetAsyncSocketServerTransport serverTransport(serverContext);
    auto server = ThingSetServerBuilder::build(serverTransport);
    server.listen();

    EXPECT_TRUE(server.publish(Subset::live));
}
