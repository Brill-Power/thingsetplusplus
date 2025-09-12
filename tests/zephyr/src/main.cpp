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

LOG_MODULE_REGISTER(thingsetplusplustest, CONFIG_THINGSET_PLUS_PLUS_LOG_LEVEL);

using namespace ThingSet::Can::Zephyr;
using namespace ThingSet;

static const struct device *canDevice = DEVICE_DT_GET(DT_CHOSEN(zephyr_canbus));

std::array<uint8_t, 1024> serverRxBuffer;
std::array<uint8_t, 1024> serverTxBuffer;
std::array<uint8_t, 1024> clientRxBuffer;
std::array<uint8_t, 1024> clientTxBuffer;
ThingSetZephyrCanInterface serverInterface(canDevice);
ThingSetZephyrCanInterface clientInterface(canDevice);
ThingSetZephyrCanServerTransport serverTransport(serverInterface, serverRxBuffer, serverTxBuffer);
ThingSetZephyrCanClientTransport clientTransport(clientInterface, 0x01, clientRxBuffer, clientTxBuffer);

ThingSetReadWriteProperty<float> totalVoltage { 0x300, 0, "totalVoltage", 24.0f };

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

#define CREATE_AND_RUN(thread, stack, function) \
    k_thread_create(&thread, stack, K_THREAD_STACK_SIZEOF(stack), function, NULL, NULL, NULL, 2, 0, K_NO_WAIT);

static k_tid_t createAndRunServer()
{
    return CREATE_AND_RUN(serverThread, serverStack, runServer);
}

static k_tid_t createAndRunClient(k_thread_entry_t runner)
{
    return CREATE_AND_RUN(clientThread, clientStack, runner);
}

// name needs to be this to make stupid twister check pass
#define ZCLIENT_SERVER_TEST(test_name, Body) \
ZTEST(ZephyrClientServer, test_name) \
{ \
    k_sem_init(&clientCompleted, 0, 1); \
    k_sem_init(&serverCompleted, 0, 1); \
\
    createAndRunServer(); \
\
    createAndRunClient([](auto, auto, auto) \
    { \
        LOG_INF("Creating client"); \
        std::array<uint8_t, 1024> localRxBuffer; \
        std::array<uint8_t, 1024> localTxBuffer; \
        ThingSetClient client(clientTransport, localRxBuffer, localTxBuffer); \
        zassert_true(client.connect()); \
        LOG_INF("Client connected"); \
\
        Body \
\
        k_sem_give(&clientCompleted); \
    }); \
\
    k_sem_take(&serverCompleted, K_FOREVER); \
}

ZCLIENT_SERVER_TEST(test_get_float,
    float tv;
    zassert_true(client.get(0x300, tv));
)

ZCLIENT_SERVER_TEST(test_exec_function,
    int result;
    zassert_true(client.exec(0x1000, &result, 2, 3));
    zassert_equal(5, result);
)

ZCLIENT_SERVER_TEST(test_update,
    zassert_true(client.update("totalVoltage", 25.0f));
    k_sleep(K_MSEC(100)); // `update` is async or something
    zassert_equal(25.0f, totalVoltage.getValue());
)

static void *testSetup(void)
{
    zassert_equal(0, can_set_mode(canDevice, CAN_MODE_LOOPBACK));
    zassert_true(serverInterface.bind(0x01));
    zassert_true(clientInterface.bind(0x02));
    return nullptr;
}

ZTEST_SUITE(ZephyrClientServer, NULL, testSetup, NULL, NULL, NULL);