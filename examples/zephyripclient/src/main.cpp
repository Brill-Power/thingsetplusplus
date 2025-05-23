/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <thingset++/ThingSet.hpp>
#include <thingset++/ThingSetClient.hpp>
#include <thingset++/ThingSetListener.hpp>
#include <thingset++/ip/zsock/ThingSetZephyrSocketClientTransport.hpp>
#include <thingset++/ip/zsock/ThingSetZephyrSocketSubscriptionTransport.hpp>
#include <zephyr/net/net_if.h>
#include <iostream>

using namespace ThingSet;
using namespace ThingSet::Ip::Zsock;

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
    ThingSetReadWriteProperty<0x609, 0x600, "supercells", std::array<SupercellRecord, 6>>
        supercells;
};

ThingSetReadWriteProperty<0x300, 0, "totalVoltage", float> totalVoltage = 24;

ThingSetReadWriteProperty<0x620, 0x0, "Modules", std::array<ModuleRecord, 2>> moduleRecords;

std::array<uint8_t, 1024> rxBuffer;
std::array<uint8_t, 1024> txBuffer;

int main()
{
    struct net_if *iface = net_if_get_default();

    char name_buf[16];
    net_if_get_name(iface, name_buf, 16);

    printk("Using iface: %s\n", name_buf);

    ThingSetZephyrSocketClientTransport clientTransport(iface, CONFIG_NET_CONFIG_PEER_IPV4_ADDR);
    ThingSetClient client(clientTransport, rxBuffer, txBuffer);
    ThingSetZephyrSocketSubscriptionTransport subscriptionTransport(iface, CONFIG_NET_CONFIG_MY_IPV4_ADDR);
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
