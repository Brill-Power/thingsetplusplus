/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "thingset++/StreamingThingSetBinaryEncoder.hpp"

namespace ThingSet {

template <size_t Size>
class StreamingTransportThingSetBinaryEncoder : public StreamingThingSetBinaryEncoder<Size>
{
private:
    _ThingSetServerTransport &_transport;

public:
    StreamingTransportThingSetBinaryEncoder(_ThingSetServerTransport &transport) : _transport(transport)
    {}

protected:
    bool write(size_t length, bool) override {
        return _transport.publish(&_buffer[0], length);
    }
};

}