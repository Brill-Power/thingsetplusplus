/*
 * Copyright (c) 2024-2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <functional>
#include <thingset++/ThingSet.hpp>
#include <thingset++/ThingSetClient.hpp>
#include <thingset++/ThingSetListener.hpp>
#include <thingset++/ip/sockets/ThingSetSocketClientTransport.hpp>
#include <thingset++/ip/sockets/ThingSetSocketSubscriptionTransport.hpp>
#include <iostream>
#include <unistd.h>

using namespace ThingSet;
using namespace ThingSet::Ip::Sockets;

ThingSetGroup<0x600, 0, "Modules"> modules;
ThingSetGroup<0x610, 0x610, "Supercells"> supercells;

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

ThingSetReadWriteProperty<float> totalVoltage { 0x300, 0, "totalVoltage", 24.0f };

ThingSetReadWriteProperty<std::array<ModuleRecord, 2>> moduleRecords { 0x620, 0x0, "Modules" };

std::array<uint8_t, 1024> rxBuffer;
std::array<uint8_t, 1024> txBuffer;

int main(int argc, char *argv[])
{
    std::string endpoint;
    if (argc > 1) {
        endpoint = argv[1];
    }
    else {
        endpoint = "127.0.0.1";
    }
    ThingSetSocketClientTransport clientTransport(endpoint);
    ThingSetClient client(clientTransport, rxBuffer, txBuffer);
    ThingSetSocketSubscriptionTransport subscriptionTransport;
    auto listener = ThingSetListenerBuilder::build(subscriptionTransport);

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

    listener.subscribe([&](auto sender, auto id) {
        std::cout << "Received report for " << id << " from " << sender << std::endl;
        for (size_t i = 0; i < moduleRecords.size(); i++)
        {
            std::cout << "Module " << i << "; voltage: " << moduleRecords[i].voltage << std::endl;
        }
    });

     while (true) {

        sleep(1);
    }

    return 0;
}