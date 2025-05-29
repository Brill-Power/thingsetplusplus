/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "thingset++/StreamingThingSetBinaryEncoder.hpp"
#include "thingset++/ThingSetStatus.hpp"

#define THINGSET_STREAMING_ENCODER_UDP_MSG_SIZE 4096

namespace ThingSet::Ip {

template <typename Identifier> class ThingSetIpServerTransport;

/// @brief Encodes data to successive UDP frames in a multi-frame report.
/// @tparam Identifier Type of client identifier
template <typename Identifier>
class StreamingUdpThingSetBinaryEncoder : public StreamingThingSetBinaryEncoder<THINGSET_STREAMING_ENCODER_UDP_MSG_SIZE>
{
private:
    ThingSetIpServerTransport<Identifier> &_transport;

public:
    StreamingUdpThingSetBinaryEncoder(ThingSetIpServerTransport<Identifier> &transport) : _transport(transport)
    {
        _buffer[0] = ThingSetRequestType::report;
        zcbor_new_encode_state(_state, BINARY_ENCODER_DEFAULT_MAX_DEPTH, &_buffer[3], _buffer.size() - 3, 2);
    }

protected:
    bool write(size_t length, bool) override
    {
        _buffer[1] = (uint8_t)length;
        _buffer[2] = (uint8_t)(length >> 8);
        return _transport.publish(_buffer.data(), length + 3);
    }
};

} // namespace ThingSet::Ip