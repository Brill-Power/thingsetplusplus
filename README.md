# ThingSet++

ThingSet framework in C++.

## Introduction

ThingSet is a transport-agnostic data accessibility framework for embedded devices. For more
information on ThingSet, please visit [the ThingSet web site](https://thingset.io/).

There are four transports in various stages of completeness and maturity:

- SocketCAN - client and server (Linux)
- Zephyr CAN - client and server (Zephyr RTOS)
- TCP/UDP Sockets - client and server (Linux, macOS (client only), FreeBSD, OpenBSD, Zephyr RTOS)
- TCP/UDP asio Sockets - client and server (Linux, macOS, FreeBSD, OpenBSD)

Both text (JSON) and binary (CBOR) encodings are supported.

## Getting Started

### Server

The basic building block is a property. A property has an ID, a parent, a name and a value of a
certain type. For example:

```c++
ThingSetReadWriteProperty<float> voltage { 0x300, 0, "voltage" };
```

The type will be inferred if the value is specified at initialisation:

```c++
ThingSetReadWriteProperty voltage { 0x300, 0, "voltage", 24.0f };
```

A property so declared provides its own storage for the given value. Alternatively,
a property may be declared with a pointer:

```c++
float voltage;
ThingSetReadWriteProperty voltageProperty { 0x300, 0, "voltage", &voltage };
```

The type will be inferred from the pointer type. Assignment to the property will update
the underlying value.

All the basic primitive types are supported. Properties may also be structures*, or arrays of primitives
or structures (known in C ThingSet as 'records').

Structures should be declared thus:

```c++
struct ModuleRecord
{
    ThingSetReadWriteRecordMember<0x601, 0x600, "voltage", float> voltage;
    ThingSetReadWriteRecordMember<0x602, 0x600, "current", float> current;
    ThingSetReadWriteRecordMember<0x603, 0x600, "error", uint64_t> error;
    ThingSetReadWriteRecordMember<0x604, 0x600, "cellVoltages", std::array<float, 6>> cellVoltages;
};
```

As in C ThingSet, the above will be serialised as a map, with either integer ID or string name keys.

Once you have declared your properties, instantiate a server with an appropriate transport to expose
them to clients.

```c++
ThingSetSocketServerTransport transport;
auto server = ThingSetServerBuilder::build(transport);
server.listen();
```

(* There is no direct equivalent of a single structure being a value in C ThingSet, which may cause
compatibility problems.)

### Client

The client is instantiated along similar lines:

```c++
std::array<uint8_t, 1024> rxBuffer;
std::array<uint8_t, 1024> txBuffer;
ThingSetSocketClientTransport clientTransport("127.0.0.1");
ThingSetClient client(clientTransport, rxBuffer, txBuffer);
client.connect();
```

Values can then be retrieved:

```c++
float voltage;
if (client.get(0x300, voltage)) {
    ...
}
```

...or updated:

```c++
client.update("voltage", 25.0f);
```

...and functions invoked:

```c++
int result;
if (client.exec(0x1000, &result, 2, 3)) {
    ...
}
```

## Examples and Tests

See the `examples` folder for some samples of the code in action. The unit tests also give an idea of what can be done.
