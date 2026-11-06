/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "thingset++/ip/sockets/ThingSetSocketServerTransport.hpp"
#include "thingset++/ip/sockets/ThingSetSocketSubscriptionTransport.hpp"
#include "thingset++/ThingSetServer.hpp"
#include "thingset++/ThingSetListener.hpp"
#include "gtest/gtest.h"
#include <atomic>
#include <chrono>
#include <thread>

using namespace ThingSet;
using namespace ThingSet::Ip::Sockets;

/// Waits up to a second for count to reach expected.
static bool waitFor(const std::atomic<int> &count, int expected)
{
    auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(1);
    while (count < expected) {
        if (std::chrono::steady_clock::now() > deadline) {
            return false;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    return true;
}

TEST(SocketIpPublishSubscribe, ReceivesPublishedReports)
{
    ThingSetReadWriteProperty<float> totalVoltage { 0x300, 0, "totalVoltage", 24.0f };
    ThingSetSocketServerTransport serverTransport;
    auto server = ThingSetServerBuilder::build(serverTransport);
    ASSERT_TRUE(server.listen());

    ThingSetSocketSubscriptionTransport subscriptionTransport;
    auto listener = ThingSetListenerBuilder::build(subscriptionTransport);
    std::atomic<int> receiveCount = 0;
    ASSERT_TRUE(listener.subscribe([&](auto, auto id) {
        if (id == 0x300) {
            receiveCount++;
        }
    }));

    for (int i = 1; i <= 5; i++) {
        ASSERT_TRUE(server.publish(totalVoltage));
        ASSERT_TRUE(waitFor(receiveCount, i)) << "report " << i;
    }
}

/// Destroying a transport whose listener thread is running must stop and
/// join that thread rather than terminating the process.
TEST(SocketIpPublishSubscribe, DestroyingSubscribedTransportStopsListener)
{
    std::atomic<int> receiveCount = 0;
    {
        ThingSetSocketSubscriptionTransport subscriptionTransport;
        auto listener = ThingSetListenerBuilder::build(subscriptionTransport);
        ASSERT_TRUE(listener.subscribe([&](auto, auto) { receiveCount++; }));
    }

    // reports published afterwards no longer reach the old callback
    ThingSetReadWriteProperty<float> totalVoltage { 0x300, 0, "totalVoltage", 24.0f };
    ThingSetSocketServerTransport serverTransport;
    auto server = ThingSetServerBuilder::build(serverTransport);
    ASSERT_TRUE(server.listen());
    ASSERT_TRUE(server.publish(totalVoltage));
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    ASSERT_EQ(0, receiveCount);
}

TEST(SocketIpPublishSubscribe, DestroyingUnsubscribedTransportIsSafe)
{
    ThingSetSocketSubscriptionTransport subscriptionTransport;
}

/// Once one subscription transport has gone, a new one can subscribe and
/// receive reports.
TEST(SocketIpPublishSubscribe, CanResubscribeWithNewTransport)
{
    ThingSetReadWriteProperty<float> totalVoltage { 0x300, 0, "totalVoltage", 24.0f };
    ThingSetSocketServerTransport serverTransport;
    auto server = ThingSetServerBuilder::build(serverTransport);
    ASSERT_TRUE(server.listen());

    for (int round = 0; round < 3; round++) {
        ThingSetSocketSubscriptionTransport subscriptionTransport;
        auto listener = ThingSetListenerBuilder::build(subscriptionTransport);
        std::atomic<int> receiveCount = 0;
        ASSERT_TRUE(listener.subscribe([&](auto, auto id) {
            if (id == 0x300) {
                receiveCount++;
            }
        })) << "round " << round;
        ASSERT_TRUE(server.publish(totalVoltage)) << "round " << round;
        ASSERT_TRUE(waitFor(receiveCount, 1)) << "round " << round;
    }
}
