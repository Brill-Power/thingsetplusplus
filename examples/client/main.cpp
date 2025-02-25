#include <asio.hpp>
#include <functional>
#include <thingset++/ThingSet.hpp>
#include <thingset++/ThingSetClient.hpp>
#include <thingset++/asio/ThingSetAsyncSocketClientTransport.hpp>

using namespace ThingSet;
using namespace ThingSet::Async;

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