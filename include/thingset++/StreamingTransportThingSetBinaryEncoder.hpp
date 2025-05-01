/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "thingset++/StreamingThingSetBinaryEncoder.hpp"
#include "thingset++/ThingSetStatus.hpp"

namespace ThingSet {

template <size_t Size>
class StreamingTransportThingSetBinaryEncoder : public StreamingThingSetBinaryEncoder<Size>
{
private:
    _ThingSetServerTransport &_transport;

public:
    StreamingTransportThingSetBinaryEncoder(_ThingSetServerTransport &transport) : StreamingThingSetBinaryEncoder<Size>::StreamingThingSetBinaryEncoder(), _transport(transport)
    {
        this->_buffer[0] = ThingSetRequestType::report;
        zcbor_new_encode_state(this->_state, BINARY_ENCODER_DEFAULT_MAX_DEPTH, &this->_buffer[3], this->_buffer.size() - 3, 2);
    }

protected:
    bool write(size_t length, bool) override {
        this->_buffer[1] = (uint8_t)length;
        this->_buffer[2] = (uint8_t)(length >> 8);
        return _transport.publish(&this->_buffer[0], length + 3);
    }
};

}