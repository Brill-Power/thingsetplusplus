# ThingSet++

ThingSet framework in C++.

## Introduction

ThingSet is a transport-agnostic data accessibility framework for embedded devices. For more
information on ThingSet, please visit [the ThingSet web site](https://thingset.io/).

There are three transports in various stages of completeness and maturity:

- SocketCAN - client and server
- Zephyr CAN - client; incomplete server
- Asio TCP/UDP - client and server

The client components have had more real-world testing than the server, which is provided as
something of a work in progress.

At present, only binary mode (CBOR) is supported.

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
