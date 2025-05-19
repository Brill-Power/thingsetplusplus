/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
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
ThingSetZephyrCanInterface serverInterface(canDevice, serverRxBuffer, serverTxBuffer);
ThingSetZephyrCanServerTransport serverTransport(serverInterface);

ThingSetReadWriteProperty<0x300, 0, "totalVoltage", float> totalVoltage = 24;

ThingSetUserFunction<0x1000, 0x0, "xAddNumber", int, int, int> doSomething([](auto x, auto y) { return x + y; });

static struct k_sem quitLock;

int main()
{
    k_sem_init(&quitLock, 0, K_SEM_MAX_LIMIT);

    serverInterface.bind(0x01);

    auto server = ThingSetServerBuilder::build(serverTransport);
    server.listen();

    LOG_INF("Started up");

    k_sem_take(&quitLock, K_FOREVER);
}