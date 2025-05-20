/*
 * Copyright (c) 2024-2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <asio.hpp>
#include <functional>
#include <iostream>
#include <thingset++/ThingSet.hpp>
#include <thingset++/ThingSetServer.hpp>
#include <thingset++/ip/asio/ThingSetAsyncSocketServerTransport.hpp>
#include <thingset++/ip/udp/StreamingUdpThingSetBinaryEncoder.hpp>

using namespace ThingSet;
using namespace ThingSet::Ip::Async;
using namespace ThingSet::Ip::Udp;

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

ThingSetUserFunction<0x1000, 0x0, "xDoSomething", int, int, int> doSomething([](auto x, auto y) { return x + y; });

void publishCallback(const asio::error_code & /*e*/, asio::steady_timer *t, ThingSetServer<asio::ip::tcp::endpoint, THINGSET_STREAMING_ENCODER_UDP_MSG_SIZE, StreamingUdpThingSetBinaryEncoder<asio::ip::tcp::endpoint>> *server)
{
    std::cout << "Publishing report" << std::endl;
    for (int i = 0; i < moduleRecords.size(); i++) {
        auto increment = 0.25 * (std::rand() % 4);
        if (std::rand() % 1 == 0) {
            increment *= -1;
        }
        moduleRecords[i].voltage += increment;
    }
    server->publish(moduleRecords, totalVoltage);

    t->expires_at(t->expiry() + asio::chrono::seconds(1));
    t->async_wait(std::bind(publishCallback, asio::placeholders::error, t, server));
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

    asio::io_context ioContext(1);
    ThingSetAsyncSocketServerTransport transport(ioContext);
    auto server = ThingSetServerBuilder::build(transport);

    asio::steady_timer t(ioContext, asio::chrono::seconds(1));
    t.async_wait(std::bind(publishCallback, asio::placeholders::error, &t, &server));

    server.start();
    ioContext.run();
    return 0;
}