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

ThingSetReadWriteProperty<uint32_t, Subset::persisted> identifier { 0x20, 0, "identifier", 1 };

ThingSetUserFunction<0x1000, 0x0, "xAddNumber", int, int, int> doSomething([](auto x, auto y) { return x + y; });

k_thread serverThread;
K_THREAD_STACK_DEFINE(serverStack, CONFIG_ARCH_POSIX_RECOMMENDED_STACK_SIZE);

k_thread clientThread;
K_THREAD_STACK_DEFINE(clientStack, CONFIG_ARCH_POSIX_RECOMMENDED_STACK_SIZE);

k_sem serverStarted;
k_sem serverCompleted;
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

static k_tid_t createAndRunServer()
{
    return CREATE_AND_RUN(serverThread, serverStack, runServer);
}

static k_tid_t createAndRunClient(k_thread_entry_t runner)
{
    return CREATE_AND_RUN(clientThread, clientStack, runner);
}

// name needs to be this to make stupid twister check pass
// variadic so test bodies may contain top-level commas (e.g. template args)
#define ZCLIENT_SERVER_TEST(test_name, ...) \
ZTEST(ZephyrClientServer, test_name) \
{ \
    k_sem_init(&serverStarted, 0, 1); \
    k_sem_init(&serverCompleted, 0, 1); \
    k_sem_init(&clientCompleted, 0, 1); \
\
    createAndRunServer(); \
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
        __VA_ARGS__ \
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

/* Transport lifecycle: repeatedly create a client to a node that never
 * answers, let the request time out, and destroy the transport. Historically
 * in-flight context handling could leak send contexts from a 4-deep slab and
 * left timers/work items pointing at destroyed (stack-allocated) transports.
 * Six cycles (> slab depth) surface a reintroduced leak as ISOTP_NO_CTX_LEFT,
 * and the final exchange proves the shared client still works. */
ZCLIENT_SERVER_TEST(test_client_lifecycle_absent_node,
    for (int i = 0; i < 6; i++) {
        std::array<uint8_t, 64> transportRx;
        std::array<uint8_t, 64> transportTx;
        std::array<uint8_t, 64> absentClientRx;
        std::array<uint8_t, 64> absentClientTx;
        ThingSetZephyrCanClientTransport absentTransport(clientInterface, 0x55, transportRx,
                                                         transportTx);
        ThingSetClient absentClient(absentTransport, absentClientRx, absentClientTx);
        zassert_true(absentClient.connect());
        int sum;
        auto absentResult = absentClient.exec(0x1000, &sum, 1, 2);
        zassert_false(absentResult.success(), "exec to an absent node must not succeed");
    }

    int value;
    auto result = client.exec(0x1000, &value, 2, 3);
    zassert_true(result.success(), "shared client must still work after lifecycle churn");
    zassert_equal(5, value);
)

/* Response cross-talk: RR client filters historically masked out the source
 * address, so every client on a shared interface matched every peer's
 * responses (first-match-wins on real hardware silently starved the loser).
 * A client bound to a silent peer must not observe responses addressed to
 * another client. */
ZCLIENT_SERVER_TEST(test_no_response_crosstalk_between_clients,
    std::array<uint8_t, 64> transportRx;
    std::array<uint8_t, 64> transportTx;
    std::array<uint8_t, 64> silentClientRx;
    std::array<uint8_t, 64> silentClientTx;
    ThingSetZephyrCanClientTransport silentTransport(clientInterface, 0x55, transportRx,
                                                     transportTx);
    ThingSetClient silentClient(silentTransport, silentClientRx, silentClientTx);
    zassert_true(silentClient.connect());

    /* a full exchange with the real server crosses the bus */
    int value;
    auto result = client.exec(0x1000, &value, 2, 3);
    zassert_true(result.success());
    zassert_equal(5, value);

    /* the silent-peer client must not have captured that response: its own
     * request must time out rather than return the stray reply */
    int sum;
    auto silentResult = silentClient.exec(0x1000, &sum, 1, 2);
    zassert_false(silentResult.success(),
                  "client bound to a silent peer must not see another client's response");
)

/* connect() historically could not fail: isotp_fast_bind ignored the result
 * of can_add_rx_filter, so a full filter table was reported as a successful
 * bind and every subsequent exchange timed out. Exhaust the controller's RX
 * filters and check that connect() now fails -- and recovers once filters
 * are freed. */
ZTEST(ZephyrClientServer, test_connect_fails_when_filters_exhausted)
{
    struct can_filter filter = {
        .id = 0x100,
        .mask = CAN_EXT_ID_MASK,
        .flags = CAN_FILTER_IDE,
    };
    int filterIds[128];
    int count = 0;

    while (count < (int)ARRAY_SIZE(filterIds)) {
        int id = can_add_rx_filter(
            canDevice, [](const struct device *, struct can_frame *, void *) {}, nullptr, &filter);
        if (id < 0) {
            break;
        }
        filterIds[count++] = id;
    }
    zassert_true(count < (int)ARRAY_SIZE(filterIds), "expected to exhaust CAN RX filters");

    std::array<uint8_t, 64> rxBuffer;
    std::array<uint8_t, 64> txBuffer;
    ThingSetZephyrCanClientTransport transport(clientInterface, 0x01, rxBuffer, txBuffer);
    zassert_false(transport.connect(), "connect() must fail with no free RX filters");

    for (int i = 0; i < count; i++) {
        can_remove_rx_filter(canDevice, filterIds[i]);
    }

    zassert_true(transport.connect(), "connect() must succeed again once filters are free");
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