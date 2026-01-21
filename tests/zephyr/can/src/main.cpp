/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <zephyr/ztest.h>

#include <thingset++/can/zephyr/ThingSetZephyrCanClientTransport.hpp>
#include <thingset++/can/zephyr/ThingSetZephyrCanServerTransport.hpp>
#include <thingset++/can/zephyr/ThingSetZephyrCanSubscriptionTransport.hpp>
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

ThingSetReadWriteProperty<uint32_t, Subset::persisted> identifier { 0x20, 0, "identifier", 1 };

ThingSetUserFunction<0x1000, 0x0, "xAddNumber", int, int, int> doSomething([](auto x, auto y) { return x + y; });

k_thread serverThread;
K_THREAD_STACK_DEFINE(serverStack, CONFIG_ARCH_POSIX_RECOMMENDED_STACK_SIZE);

k_thread clientThread;
K_THREAD_STACK_DEFINE(clientStack, CONFIG_ARCH_POSIX_RECOMMENDED_STACK_SIZE);

k_sem serverStarted;
k_sem serverCompleted;
k_sem clientStarted;
k_sem clientCompleted;

static void runServer(void *, void *, void *)
{
    LOG_INF("Server starting up");
    auto server = ThingSetServerBuilder::build(serverTransport);
    server.listen();
    k_sem_give(&serverStarted);
    k_sem_take(&clientCompleted, K_FOREVER);
    LOG_INF("Server shutting down");
    k_sem_give(&serverCompleted);
}

#define CREATE_AND_RUN(thread, stack, function) \
    k_thread_create(&thread, stack, K_THREAD_STACK_SIZEOF(stack), function, NULL, NULL, NULL, 2, 0, K_NO_WAIT);

static k_tid_t createAndRunServer(k_thread_entry_t runner)
{
    return CREATE_AND_RUN(serverThread, serverStack, runner);
}

static k_tid_t createAndRunClient(k_thread_entry_t runner)
{
    return CREATE_AND_RUN(clientThread, clientStack, runner);
}

// name needs to be this to make stupid twister check pass
#define ZCLIENT_SERVER_TEST(test_name, Body) \
ZTEST(ZephyrClientServer, test_name) \
{ \
    k_sem_init(&serverStarted, 0, 1); \
    k_sem_init(&serverCompleted, 0, 1); \
    k_sem_init(&clientCompleted, 0, 1); \
\
    createAndRunServer(runServer); \
\
    k_sem_take(&serverStarted, K_FOREVER); \
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
    auto result = client.get(0x300, tv);
    zassert_true(result.success());
    LOG_INF("result.code: 0x%x", result.code());
    zassert_equal(ThingSetStatusCode::content, result.code());
)

ZCLIENT_SERVER_TEST(test_exec_function,
    int value;
    auto result = client.exec(0x1000, &value, 2, 3);
    zassert_true(result.success());
    LOG_INF("result.code: 0x%x", result.code());
    zassert_equal(ThingSetStatusCode::changed, result.code());
    zassert_equal(5, value);
)

ZCLIENT_SERVER_TEST(test_update,
    auto result = client.update("totalVoltage", 25.0f);
    zassert_true(result.success());
    LOG_INF("result.code: 0x%x", result.code());
    zassert_equal(ThingSetStatusCode::changed, result.code());
    k_sleep(K_MSEC(100)); // `update` is async or something
    zassert_equal(25.0f, totalVoltage.getValue());
)

ZTEST(ZephyrClientServer, test_publish_subscribe)
{
    k_sem_init(&serverStarted, 0, 1);
    k_sem_init(&serverCompleted, 0, 1);
    k_sem_init(&clientStarted, 0, 1);
    k_sem_init(&clientCompleted, 0, 1);

    createAndRunServer([](auto, auto, auto)
    {
        LOG_INF("Server starting up");
        auto server = ThingSetServerBuilder::build(serverTransport);
        server.listen();
        k_sem_give(&serverStarted);
        k_sem_take(&clientStarted, K_FOREVER);
        server.publish(identifier, totalVoltage);
        k_sem_take(&clientCompleted, K_FOREVER);
        LOG_INF("Server shutting down");
        k_sem_give(&serverCompleted);
    });

    k_sem_take(&serverStarted, K_FOREVER);

    createAndRunClient([](auto, auto, auto)
    {
        LOG_INF("Creating client");
        std::array<uint8_t, 1024> localRxBuffer;
        std::array<uint8_t, 1024> localTxBuffer;
        ThingSetClient client(clientTransport, localRxBuffer, localTxBuffer);
        zassert_true(client.connect());
        LOG_INF("Client connected");

        k_sem publicationReceived;
        k_sem_init(&publicationReceived, 0, 1);
        ThingSetZephyrCanSubscriptionTransport subscriptionTransport(clientInterface);
        subscriptionTransport.subscribe([&](auto canId, auto decoder)
        {
            if (decoder.decodeMap([&](auto id, auto name)
            {
                LOG_INF("Key %x", id.value());
                return decoder.skip();
            })) {
                k_sem_give(&publicationReceived);
            }
        });
        k_sem_give(&clientStarted);
        k_sem_take(&publicationReceived, K_FOREVER);

        k_sem_give(&clientCompleted);
    });

    k_sem_take(&serverCompleted, K_FOREVER);
}

static void *testSetup(void)
{
    // Not allowed until interface is bound to address
    zassert_false(clientInterface.claimAddress());

    zassert_equal(0, can_set_mode(canDevice, CAN_MODE_LOOPBACK));
    zassert_true(serverInterface.bind(0x01));
    zassert_true(clientInterface.bind(0x02));

    // Now we should be able to do this multiple times
    zassert_true(clientInterface.claimAddress());
    zassert_true(clientInterface.claimAddress());

    return nullptr;
}

ZTEST_SUITE(ZephyrClientServer, NULL, testSetup, NULL, NULL, NULL);