/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <thingset++/can/zephyr/ThingSetZephyrCanServerTransport.hpp>
#include <thingset++/ThingSetServer.hpp>
#include <thingset++/ThingSetFunction.hpp>
#include <array>

using namespace ThingSet::Can::Zephyr;
using namespace ThingSet;

static const struct device *canDevice = DEVICE_DT_GET(DT_CHOSEN(zephyr_canbus));

std::array<uint8_t, 1024> rxBuffer;
std::array<uint8_t, 1024> txBuffer;
ThingSetZephyrCanInterface interface(canDevice);
ThingSetZephyrCanServerTransport transport(interface, rxBuffer, txBuffer);

struct SupercellRecord
{
    ThingSetReadWriteRecordMember<0x611, 0x610, "soc", float> soc;
    ThingSetReadWriteRecordMember<0x612, 0x610, "soh", float> soh;
};

struct ModuleRecord
{
    ThingSetReadWriteRecordMember<0x601, 0x600, "voltage", float> voltage;
    ThingSetReadWriteRecordMember<0x602, 0x600, "current", float> current;
    ThingSetReadWriteRecordMember<0x603, 0x600, "error", uint64_t> error;
    ThingSetReadWriteRecordMember<0x604, 0x600, "cellVoltages", std::array<float, 6>> cellVoltages;
    ThingSetReadWriteRecordMember<0x609, 0x600, "supercells", std::array<SupercellRecord, 6>> supercells;
};

ThingSetReadOnlyProperty<std::array<ModuleRecord, 2>> moduleRecords { 0x600, 0, "Modules" };

ThingSetReadWriteProperty<float> totalVoltage { 0x300, 0, "totalVoltage", 24.0f };

ThingSetUserFunction<0x1000, 0x0, "xAddNumber", int, int, int> doSomething([](auto x, auto y) { return x + y; });

k_sem quitLock;

k_thread publishThread;
K_THREAD_STACK_DEFINE(publishStack, CONFIG_ARCH_POSIX_RECOMMENDED_STACK_SIZE);

bool runPublisher = true;

void publisher(void *param1, void*, void *)
{
    auto server = (ThingSetServer<Can::CanID, CAN_MAX_DLEN, Can::StreamingCanThingSetBinaryEncoder> *)param1;
    while (runPublisher)
    {
        for (size_t i = 0; i < moduleRecords.size(); i++) {
            auto increment = 0.25f * (std::rand() % 4);
            if (std::rand() % 1 == 0) {
                increment *= -1;
            }
            moduleRecords[i].voltage += increment;
        }
        server->publish(moduleRecords, totalVoltage);
        LOG_INF("Publishing report");
        k_sleep(K_SECONDS(1));
    }
}

int main()
{
    moduleRecords = { (ModuleRecord){
                          .voltage = 24.0f,
                          .current = 10.0f,
                          .error = 0,
                          .cellVoltages = { { 3.2f, 3.1f, 2.9f, 3.3f, 0.0f, 2.8f } },
                          .supercells = { { (SupercellRecord){
                                                .soc = 0.1,
                                                .soh = 1,
                                            },
                                            (SupercellRecord){ .soc = 0.25, .soh = 1 },
                                            (SupercellRecord){
                                                .soc = 0.5,
                                                .soh = 1,
                                            },
                                            (SupercellRecord){
                                                .soc = 0.75,
                                                .soh = 1,
                                            },
                                            (SupercellRecord){
                                                .soc = 0.8,
                                                .soh = 1,
                                            },
                                            (SupercellRecord){
                                                .soc = 1,
                                                .soh = 1,
                                            } } },
                      },
                      (ModuleRecord){
                          .voltage = 24.2f,
                          .current = 5.0f,
                          .error = 0,
                          .cellVoltages = { { 3.1f, 3.3f, 3.0f, 3.1f, 3.2f, 2.95f } },
                      } };

    k_sem_init(&quitLock, 0, K_SEM_MAX_LIMIT);

    interface.bind(0x01);

    auto server = ThingSetServerBuilder::build(transport);
    server.listen();

    k_thread_create(&publishThread, publishStack, K_THREAD_STACK_SIZEOF(publishStack), publisher, &server, nullptr, nullptr, 2, 0, K_NO_WAIT);

    LOG_INF("Started up");

    k_sem_take(&quitLock, K_FOREVER);
}