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

struct SupercellRecord
{
    ThingSetReadOnlyProperty<0x611, 0x610, "soc", float> soc;
    ThingSetReadOnlyProperty<0x612, 0x610, "soh", float> soh;
};

struct ModuleRecord
{
    ThingSetReadOnlyProperty<0x601, 0x600, "voltage", float> voltage;
    ThingSetReadOnlyProperty<0x602, 0x600, "current", float> current;
    ThingSetReadOnlyProperty<0x603, 0x600, "error", uint64_t> error;
    ThingSetReadOnlyProperty<0x604, 0x600, "cellVoltages", std::array<float, 6>> cellVoltages;
    ThingSetReadOnlyProperty<0x610, 0x600, "supercells", std::array<SupercellRecord, 6>> supercells;
};

ThingSetReadOnlyProperty<0x600, 0, "Modules", std::array<ModuleRecord, 2>> moduleRecords;

ThingSetReadWriteProperty<0x300, 0, "totalVoltage", float> totalVoltage = 24;

ThingSetUserFunction<0x1000, 0x0, "xAddNumber", int, int, int> doSomething([](auto x, auto y) { return x + y; });

k_sem quitLock;

k_thread publishThread;
K_THREAD_STACK_DEFINE(publishStack, CONFIG_ARCH_POSIX_RECOMMENDED_STACK_SIZE);

bool runPublisher = true;

void publisher(void * param1, void*, void *)
{
    auto server = (ThingSetServer<Can::CanID, CAN_MAX_DLEN, Can::StreamingCanThingSetBinaryEncoder> *)param1;
    while (runPublisher)
    {
        server->publish(moduleRecords, totalVoltage);
        LOG_INF("Publishing report");
        k_sleep(K_SECONDS(1));
    }
}

int main()
{
    k_sem_init(&quitLock, 0, K_SEM_MAX_LIMIT);

    serverInterface.bind(0x01);

    auto server = ThingSetServerBuilder::build(serverTransport);
    server.listen();

    k_thread_create(&publishThread, publishStack, K_THREAD_STACK_SIZEOF(publishStack), publisher, &server, nullptr, nullptr, 2, 0, K_NO_WAIT);

    LOG_INF("Started up");

    k_sem_take(&quitLock, K_FOREVER);
}