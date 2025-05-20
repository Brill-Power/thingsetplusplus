/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <thingset++/can/zephyr/ThingSetZephyrCanClientTransport.hpp>
#include <thingset++/can/zephyr/ThingSetZephyrCanSubscriptionTransport.hpp>
#include <thingset++/ThingSetClient.hpp>
#include <thingset++/ThingSetProperty.hpp>
#include <thingset++/ThingSetFunction.hpp>
#include <thingset++/ThingSetListener.hpp>
#include <array>

using namespace ThingSet::Can::Zephyr;
using namespace ThingSet;

static const struct device *canDevice = DEVICE_DT_GET(DT_CHOSEN(zephyr_canbus));

std::array<uint8_t, 1024> rxBuffer;
std::array<uint8_t, 1024> txBuffer;
ThingSetZephyrCanInterface interface(canDevice, rxBuffer, txBuffer);
ThingSetZephyrCanClientTransport transport(interface, 0x01);
ThingSetZephyrCanSubscriptionTransport subscriptionTransport(interface);

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

int main()
{
    k_sem_init(&quitLock, 0, K_SEM_MAX_LIMIT);

    interface.bind(0x02);

    std::array<uint8_t, 1024> localRxBuffer;
    std::array<uint8_t, 1024> localTxBuffer;
    ThingSetClient client(transport, localRxBuffer, localTxBuffer);

    client.connect();

    // gets the value of voltage
    float voltage;
    if (client.get(0x300, voltage)) {
        std::cout << "Voltage: " << voltage << std::endl;
    }

    // invokes a method which adds 2 + 3 and returns the result
    int result;
    if (client.exec(0x1000, &result, 2, 3)) {
        std::cout << "Executed: " << result << std::endl;
    }

    auto listener = ThingSetListenerBuilder::build(subscriptionTransport);
    auto callback = [&](auto sender, auto id) {
        std::cout << "Received report for " << id << " from " << sender << std::endl;
        for (size_t i = 0; i < moduleRecords.size(); i++)
        {
            std::cout << "Module " << i << "; voltage: " << moduleRecords[i].voltage << std::endl;
        }
    };
    listener.subscribe(callback);

    LOG_INF("Started up");

    k_sem_take(&quitLock, K_FOREVER);
}