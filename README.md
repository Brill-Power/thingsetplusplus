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

The basic building block is a property. A property has an ID, a parent, a name and a type. For
example:

```c++
ThingSetReadWriteProperty<float> voltage = 24 { 0x300, 0, "voltage" };
```

A property so declared provides its own storage for the given value. Alternatively,
a property may be declared with a pointer:

```c++
float voltage;
ThingSetReadWriteProperty<float *> voltageProperty(&voltage) { 0x300, 0, "voltage" };
```

Assignment to the property will update the underlying value.

All the basic primitive types are supported. Properties may also be structures*, or arrays of primitives
or structures (known in C ThingSet as 'records').

Structures should be declared thus:

```c++
struct ModuleRecord
{
    ThingSetReadOnlyProperty<float> voltage { 0x601, 0x600, "voltage" };
    ThingSetReadOnlyProperty<float> current { 0x602, 0x600, "current" };
    ThingSetReadOnlyProperty<uint64_t> error { 0x603, 0x600, "error" };
    ThingSetReadOnlyProperty<std::array<float, 6>> cellVoltages { 0x604, 0x600, "cellVoltages" };
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

## Decoding

The decoder adopts a typical object-oriented approach. The various overloads of `decode`
handle most primitive types, C and C++ (i.e. `std::array`) arrays and appropriately-designed complex
types. Other methods decode maps (`decodeMap`) and lists into a tuple or via a callback for each
element (`decodeList`).

## Encoding

As with the decoder, the encoder adopts a similar object-oriented approach. Various overloads of
`encode` handle most types. `encodeListStart` and `encodeMapStart` are provided to allow greater
control over the encoding process.

## Streaming

Of particular interest may be the `Streaming...` variants of the encoder and decoder. These allow
efficient encoding and decoding of streams of data with very little memory footprint. The
`StreamingCanThingSetBinaryEncoder`, in particular, can encode lengthy multi-frame CAN reports while
requiring only 128 bytes (i.e. 2 CAN-FD frames' worth) of buffer space.

## Examples and Tests

See the `examples` folder for some samples of the code in action. The unit tests also give an idea of what can be done.
