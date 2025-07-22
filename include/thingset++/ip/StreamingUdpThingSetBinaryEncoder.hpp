/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "thingset++/StreamingThingSetBinaryEncoder.hpp"
#include "thingset++/ThingSetStatus.hpp"
#include "thingset++/ip/StreamingUdpConstants.hpp"

namespace ThingSet::Ip {

template <typename Identifier> class ThingSetIpServerTransport;

/// @brief Encodes data to successive UDP frames in a multi-frame report.
/// @tparam Identifier Type of client identifier
template <typename Identifier>
class StreamingUdpThingSetBinaryEncoder : public StreamingThingSetBinaryEncoder<THINGSET_STREAMING_MSG_SIZE>
{
private:
    ThingSetIpServerTransport<Identifier> &_transport;
    uint8_t _sequenceNumber;

public:
    StreamingUdpThingSetBinaryEncoder(ThingSetIpServerTransport<Identifier> &transport) : _transport(transport), _sequenceNumber(0)
    {
        _buffer[2] = (uint8_t)ThingSetBinaryRequestType::report;
        zcbor_new_encode_state(_state, BINARY_ENCODER_DEFAULT_MAX_DEPTH, &_buffer[3], _buffer.size() - 3, 2);
    }

protected:
    size_t headerSize() const override
    {
        return THINGSET_STREAMING_HEADER_SIZE;
    }

    bool write(size_t length, bool flushing) override
    {
        MessageType type = _exportedLength == 0 ? (flushing ? MessageType::single : MessageType::first) :
            (flushing ? MessageType::last : MessageType::consecutive);
        _buffer[0] = ((uint8_t)type) | (_sequenceNumber++ & THINGSET_STREAMING_SEQUENCE_NUM_MASK);
        // publish method will write messageNumber
        return _transport.publish(_buffer.data(), length + THINGSET_STREAMING_HEADER_SIZE);
    }
};

} // namespace ThingSet::Ip