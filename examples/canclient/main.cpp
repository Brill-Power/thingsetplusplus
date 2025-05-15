/*
 * Copyright (c) 2024-2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "thingset++/Eui.hpp"
#include "thingset++/ThingSet.hpp"
#include "thingset++/ThingSetClient.hpp"
#include "thingset++/can/socketcan/ThingSetSocketCanClientTransport.hpp"
#include "thingset++/can/socketcan/ThingSetSocketCanSubscriptionTransport.hpp"
#include "thingset++/ThingSetListener.hpp"
#include <functional>
#include <iostream>

using namespace ThingSet;
using namespace ThingSet::Can::SocketCan;

static bool onConfigChange(ThingSetNode *node, ThingSetCallbackReason reason)
{
    std::cout << "Node " << node->getName() << " " << reason << std::endl;
    return true;
}

ThingSetGroup<0x100, 0x0, "Config"> configGroup(onConfigChange);
ThingSetReadWriteProperty<0x101, 0x100, "sMinVoltage", float> minVoltage;
ThingSetReadWriteProperty<0x102, 0x100, "sMaxVoltage", float> maxVoltage;

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

static inline ThingSetReadOnlyProperty<0x1d, 0, "NodeID", std::string> nodeId = Eui::getString();

ThingSetReadWriteProperty<0x300, 0, "totalVoltage", float> totalVoltage = 24;

static void test(int argi, float argf)
{
    printf("int is %d, float is %f\n", argi, argf);
}

ThingSetUserFunction<0x400, 0x0, "xTest", void, int, float> xTestFunc(test);
// ThingSetParameter<0x401, 0x400, "xTestInt", int> intArg;
// ThingSetParameter<0x402, 0x400, "xTestFloat", float> floatArg;

ThingSetReadOnlyProperty<0x600, 0, "Modules", std::array<ModuleRecord, 2>> moduleRecords;

std::array<uint8_t, 1024> rxBuffer;
std::array<uint8_t, 1024> txBuffer;

int main()
{
    ThingSetSocketCanInterface interface("vcan0");
    interface.bind();
    std::cout << "Successfully bound" << std::endl;

    std::cout << "Creating client...";
    ThingSetSocketCanClientTransport clientTransport(interface, 0x01);
    ThingSetClient client(clientTransport, rxBuffer, rxBuffer);
    if (client.connect()) {
        std::cout << "done" << std::endl;
    } else {
        std::cout << "failed" << std::endl;
        return -1;
    }
    float tv;
    if (client.get(0x300, tv)) {
        std::cout << "Got total voltage " << tv << std::endl;
    }

    std::cout << "Creating subscriber...";
    ThingSetSocketCanSubscriptionTransport subscriptionTransport(interface);
    auto listener = ThingSetListenerBuilder::build(subscriptionTransport);
    // ThingSetSocketCanClientTransport transport(interface);
    // ThingSetClient client(transport, rxBuffer, txBuffer);
    listener.subscribe([&](auto sender, auto id) {
        std::cout << "Received report for " << id << " from " << sender << std::endl;
        for (size_t i = 0; i < moduleRecords.size(); i++)
        {
            std::cout << "Module " << i << "; voltage: " << moduleRecords[i].voltage << std::endl;
        }
    });
    std::cout << "done" << std::endl;

    sleep(-1);

    return 0;
}