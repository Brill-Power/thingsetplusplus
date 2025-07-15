/*
 * Copyright (c) 2024-2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "thingset++/Eui.hpp"
#include "thingset++/ThingSet.hpp"
#include "thingset++/ThingSetServer.hpp"
#include "thingset++/can/socketcan/ThingSetSocketCanServerTransport.hpp"
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

    ThingSetSocketCanInterface interface("vcan0");
    interface.bind(0x01);

    ThingSetSocketCanServerTransport transport(interface);
    auto server = ThingSetServerBuilder::build(transport);

    std::thread publisher([&]() {
        while (true) {
            sleep(1);
            std::cout << "Publishing report" << std::endl;
            for (size_t i = 0; i < moduleRecords.size(); i++) {
                auto increment = 0.25 * (std::rand() % 4);
                if (std::rand() % 1 == 0) {
                    increment *= -1;
                }
                moduleRecords[i].voltage += increment;
            }
            server.publish(moduleRecords);
        }
    });

    server.listen();
    return 0;
}