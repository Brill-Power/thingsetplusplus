/*
 * Copyright (c) 2024-2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <asio.hpp>
#include <functional>
#include <thingset++/ThingSet.hpp>
#include <thingset++/ThingSetClient.hpp>
#include <thingset++/asio/ThingSetAsyncSocketClientTransport.hpp>
#include <iostream>

using namespace ThingSet;
using namespace ThingSet::Async;

ThingSetGroup<0x600, 0, "Modules"> modules;
ThingSetGroup<0x610, 0x610, "Supercells"> supercells;

struct SupercellRecord
{
    ThingSetReadWriteProperty<0x611, 0x610, "soc", float> soc;
    ThingSetReadWriteProperty<0x612, 0x610, "soh", float> soh;
};

struct ModuleRecord
{
    ThingSetReadWriteProperty<0x601, 0x600, "voltage", float> voltage;
    ThingSetReadWriteProperty<0x602, 0x600, "current", float> current;
    ThingSetReadWriteProperty<0x603, 0x600, "error", uint64_t> error;
    ThingSetReadWriteProperty<0x604, 0x600, "cellVoltages", std::array<float, 6>> cellVoltages;
    ThingSetReadWriteProperty<0x609, 0x600, "supercells", std::array<SupercellRecord, 6>> supercells;
};

ThingSetReadWriteProperty<0x300, 0, "totalVoltage", float> totalVoltage = 24;

ThingSetReadWriteProperty<0x620, 0x0, "Modules", std::array<ModuleRecord, 2>> moduleRecords;

std::array<uint8_t, 1024> rxBuffer;
std::array<uint8_t, 1024> txBuffer;

int main()
{
    auto endpoint = asio::ip::tcp::endpoint(asio::ip::address_v4::loopback(), 9001);
    ThingSetAsyncSocketClientTransport transport(endpoint);
    ThingSetClient client(transport, rxBuffer, txBuffer);

    asio::signal_set signals(transport.getContext(), SIGINT, SIGTERM);
    signals.async_wait([&](auto, auto) { transport.getContext().stop(); });

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

    client.subscribe([&](auto id) {
        for (size_t i = 0; i < moduleRecords.size(); i++)
        {
            std::cout << "Module " << i << "; voltage: " << moduleRecords[i].voltage << std::endl;
        }

    });

    transport.getContext().run();
    return 0;
}