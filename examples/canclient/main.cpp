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
ThingSetReadWriteProperty<float> minVoltage { 0x101, 0x100, "sMinVoltage" };
ThingSetReadWriteProperty<float> maxVoltage { 0x102, 0x100, "sMaxVoltage" };

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

static inline ThingSetReadOnlyProperty<std::string> nodeId = Eui::getString() { 0x1d, 0, "NodeID" };

ThingSetReadWriteProperty<float> totalVoltage { 0x300, 0, "totalVoltage", 24.0f };

static void test(int argi, float argf)
{
    printf("int is %d, float is %f\n", argi, argf);
}

ThingSetUserFunction<0x400, 0x0, "xTest", void, int, float> xTestFunc(test);
// ThingSetParameter<0x401, 0x400, "xTestInt", int> intArg;
// ThingSetParameter<0x402, 0x400, "xTestFloat", float> floatArg;

ThingSetReadOnlyProperty<std::array<ModuleRecord, 2>> moduleRecords { 0x600, 0, "Modules" };

std::array<uint8_t, 1024> rxBuffer;
std::array<uint8_t, 1024> txBuffer;

ThingSetSocketCanInterface interface("vcan0");
ThingSetSocketCanClientTransport clientTransport(interface, 0x01);
ThingSetClient client(clientTransport, rxBuffer, rxBuffer);
ThingSetSocketCanSubscriptionTransport subscriptionTransport(interface);

int main()
{
    interface.bind();
    std::cout << "Successfully bound" << std::endl;

    std::cout << "Creating client...";

    if (client.connect()) {
        std::cout << "done" << std::endl;
    } else {
        std::cout << "failed" << std::endl;
        return -1;
    }
    float tv;
    if (client.get(0x300, tv)) {
        std::cout << "Got total voltage " << tv << std::endl;
    } else {
        std::cout << "Failed to get total voltage" << std::endl;
        return -1;
    }

    std::cout << "Creating subscriber...";
    auto listener = ThingSetListenerBuilder::build(subscriptionTransport);
    // ThingSetSocketCanClientTransport transport(interface);
    // ThingSetClient client(transport, rxBuffer, txBuffer);
    auto callback = [&](auto sender, auto id) {
        std::cout << "Received report for " << id << " from " << sender << std::endl;
        for (size_t i = 0; i < moduleRecords.size(); i++)
        {
            std::cout << "Module " << i << "; voltage: " << moduleRecords[i].voltage << std::endl;
        }
    };
    listener.subscribe(callback);
    std::cout << "done" << std::endl;

    sleep(-1);

    return 0;
}