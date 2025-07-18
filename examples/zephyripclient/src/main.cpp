/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <thingset++/ThingSet.hpp>
#include <thingset++/ThingSetClient.hpp>
#include <thingset++/ThingSetListener.hpp>
#include <thingset++/ip/sockets/ThingSetSocketClientTransport.hpp>
#include <thingset++/ip/sockets/ThingSetSocketSubscriptionTransport.hpp>
#include <zephyr/net/net_if.h>
#include <iostream>

#define PEER_IPV4_ADDR "192.0.2.2"

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

int main()
{
    struct net_if *iface = net_if_get_default();

    char name_buf[16];
    net_if_get_name(iface, name_buf, 16);

    printk("Using iface: %s\n", name_buf);

    ThingSetSocketClientTransport clientTransport(PEER_IPV4_ADDR);
    ThingSetClient client(clientTransport, rxBuffer, txBuffer);
    ThingSetSocketSubscriptionTransport subscriptionTransport;
    auto listener = ThingSetListenerBuilder::build(subscriptionTransport);

    client.connect();

    float voltage;
    if (client.get(0x300, voltage)) {
        printk("Voltage: %1.3f\n", (double)voltage);
    }

    int result;
    if (client.exec(0x1000, &result, 2, 3)) {
        printk("Executed: %d\n", result);
    }

    listener.subscribe([&](auto sender, auto id) {
        std::cout << "Received report for 0x" << std::hex << id << " from " << sender << std::endl;
        for (size_t i = 0; i < moduleRecords.size(); i++) {
            printk("Module %d; voltage: %1.3f\n", i, moduleRecords[i].voltage);
        }
    });

    while (1) {

        k_sleep(K_SECONDS(1));
    }

    return 0;
}
