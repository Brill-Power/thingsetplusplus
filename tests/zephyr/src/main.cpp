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

k_thread serverThread;
K_THREAD_STACK_DEFINE(serverStack, CONFIG_ARCH_POSIX_RECOMMENDED_STACK_SIZE);

k_thread clientThread;
K_THREAD_STACK_DEFINE(clientStack, CONFIG_ARCH_POSIX_RECOMMENDED_STACK_SIZE);

k_sem clientCompleted;
k_sem serverCompleted;

static void runServer(void *, void *, void *)
{
    LOG_INF("Server starting up");
    auto server = ThingSetServerBuilder::build(serverTransport);
    server.listen();
    k_sem_take(&clientCompleted, K_FOREVER);
    LOG_INF("Server shutting down");
    k_sem_give(&serverCompleted);
}

static k_tid_t createAndRunServer()
{
    return k_thread_create(&serverThread, serverStack, K_THREAD_STACK_SIZEOF(serverStack), runServer, NULL, NULL, NULL, CONFIG_CAN_NATIVE_LINUX_RX_THREAD_PRIORITY, 0, K_NO_WAIT);
}

static k_tid_t createAndRunClient(k_thread_entry_t runner)
{
    return k_thread_create(&clientThread, clientStack, K_THREAD_STACK_SIZEOF(clientStack), runner, NULL, NULL, NULL, CONFIG_CAN_NATIVE_LINUX_RX_THREAD_PRIORITY, 0, K_NO_WAIT);
}

ZTEST(ZephyrClientServer, Basic)
{
    k_sem_init(&clientCompleted, 0, 1);
    k_sem_init(&serverCompleted, 0, 1);

    createAndRunServer();

    createAndRunClient([](auto, auto, auto)
    {
        LOG_INF("Creating client");
        std::array<uint8_t, 1024> clientRxBuffer;
        std::array<uint8_t, 1024> clientTxBuffer;
        ThingSetClient client(clientTransport, clientRxBuffer, clientTxBuffer);
        zassert_true(client.connect());
        LOG_INF("Client connected");

        float tv;
        zassert_true(client.get(0x300, tv));

        k_sem_give(&clientCompleted);
    });

    k_sem_take(&serverCompleted, K_FOREVER);
}

static void *testSetup(void)
{
    zassert_equal(0, can_set_mode(canDevice, CAN_MODE_LOOPBACK));
    zassert_true(serverInterface.bind(0x01));
    zassert_true(clientInterface.bind(0x02));
    return nullptr;
}

ZTEST_SUITE(ZephyrClientServer, NULL, testSetup, NULL, NULL, NULL);