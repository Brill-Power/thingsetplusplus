#include <ThingSet.hpp>
#include <ThingSetClient.hpp>
#include <asio.hpp>
#include <asio/ThingSetAsyncSocketClientTransport.hpp>
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

int main()
{
    moduleRecords[0].voltage = 23.9;
    moduleRecords[0].current = 15;
    auto endpoint = asio::ip::tcp::endpoint(asio::ip::address_v4::loopback(), 9001);
    ThingSetAsyncSocketClientTransport transport(endpoint);
    ThingSetClient client(transport);

    asio::signal_set signals(transport.getContext(), SIGINT, SIGTERM);
    signals.async_wait([&](auto, auto) { transport.getContext().stop(); });

    client.connect();
    client.subscribe(&moduleRecords);

    transport.getContext().run();
    return 0;
}