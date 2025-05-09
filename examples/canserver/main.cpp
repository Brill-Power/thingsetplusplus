/*
 * Copyright (c) 2024-2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "thingset++/Eui.hpp"
#include "thingset++/ThingSet.hpp"
#include "thingset++/ThingSetServer.hpp"
#include "thingset++/can/StreamingCanThingSetBinaryEncoder.hpp"
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

    ThingSetSocketCanServerTransport transport("vcan0");
    ThingSetServer server(transport);

    std::thread publisher([&]() {
        while (true) {
            sleep(5);
            Can::StreamingCanThingSetBinaryEncoder encoder(transport);
            encoder.encode(0x0);
            encoder.encodeMapStart(1);
            encoder.encodePair(0x650, moduleRecords.getValue());
            encoder.encodeMapEnd();
            encoder.flush();
        }
    });

    server.listen();
    return 0;
}