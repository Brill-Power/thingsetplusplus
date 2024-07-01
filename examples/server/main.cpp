#include <ThingSet.hpp>
#include <ThingSetAsyncSocketServerTransport.hpp>
#include <ThingSetServer.hpp>
#include <asio.hpp>
#include <functional>

using namespace ThingSet;

ThingSetGroup<0x600, 0, "Modules"> modules;
ThingSetGroup<0x610, 0x610, "Supercells"> supercells;

struct SupercellRecord
{
    ThingSetProperty<0x611, 0x610, "soc", float> soc;
    ThingSetProperty<0x612, 0x610, "soh", float> soh;
};

struct ModuleRecord
{
    ThingSetProperty<0x601, 0x600, "voltage", float> voltage;
    ThingSetProperty<0x602, 0x600, "current", float> current;
    ThingSetProperty<0x603, 0x600, "error", uint64_t> error;
    ThingSetProperty<0x604, 0x600, "cellVoltages", std::array<float, 6>> cellVoltages;
    ThingSetProperty<0x609, 0x600, "supercells", std::array<SupercellRecord, 6>> supercells;
};

ThingSetProperty<0x300, 0, "totalVoltage", float> totalVoltage = 24;

std::array<ModuleRecord, 2> moduleRecords;

void publishCallback(const asio::error_code & /*e*/, asio::steady_timer *t, ThingSetServer *server)
{
    server->publish(0x600, moduleRecords);

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

    ThingSetAsyncSocketServerTransport transport;
    ThingSetServer server(transport);

    asio::steady_timer t(transport.getContext(), asio::chrono::seconds(1));
    t.async_wait(std::bind(publishCallback, asio::placeholders::error, &t, &server));

    server.listen();
    return 0;
}