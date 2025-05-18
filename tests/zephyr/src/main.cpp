/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <zephyr/ztest.h>

#include <thingset++/can/zephyr/ThingSetZephyrCanClientTransport.hpp>
#include <thingset++/can/zephyr/ThingSetZephyrCanServerTransport.hpp>
#include <thingset++/ThingSetClient.hpp>
#include <thingset++/ThingSetServer.hpp>
#include <thingset++/ThingSetFunction.hpp>
#include <array>

using namespace ThingSet::Can::Zephyr;
using namespace ThingSet;

static const struct device *canDevice = DEVICE_DT_GET(DT_CHOSEN(zephyr_canbus));

std::array<uint8_t, 1024> serverRxBuffer;
std::array<uint8_t, 1024> serverTxBuffer;
std::array<uint8_t, 1024> clientRxBuffer;
std::array<uint8_t, 1024> clientTxBuffer;
ThingSetZephyrCanInterface serverInterface(canDevice, serverRxBuffer, serverTxBuffer);
ThingSetZephyrCanInterface clientInterface(canDevice, clientRxBuffer, clientTxBuffer);
ThingSetZephyrCanServerTransport serverTransport(serverInterface);
ThingSetZephyrCanClientTransport clientTransport(clientInterface, 0x01);

ThingSetReadWriteProperty<0x300, 0, "totalVoltage", float> totalVoltage = 24;

ThingSetUserFunction<0x1000, 0x0, "xAddNumber", int, int, int> doSomething([](auto x, auto y) { return x + y; });

K_MSGQ_DEFINE(blockingQueue, sizeof(uint8_t), 1, 1);

ZTEST(ZephyrClientServer, Basic)
{
    auto server = ThingSetServerBuilder::build(serverTransport);
    server.listen();

    std::array<uint8_t, 1024> clientRxBuffer;
    std::array<uint8_t, 1024> clientTxBuffer;
    ThingSetClient client(clientTransport, clientRxBuffer, clientTxBuffer);
    zassert_true(client.connect());

    // float tv;
    // zassert_true(client.get(0x300, tv));

    k_sleep(K_SECONDS(30));
}

static void *testSetup(void)
{
    can_set_mode(canDevice, CAN_MODE_LOOPBACK);
    serverInterface.bind(0x01);
    clientInterface.bind(0x02);
    return nullptr;
}

ZTEST_SUITE(ZephyrClientServer, NULL, testSetup, NULL, NULL, NULL);