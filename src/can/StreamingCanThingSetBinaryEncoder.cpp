/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "thingset++/can/StreamingCanThingSetBinaryEncoder.hpp"
#include "thingset++/ThingSetStatus.hpp"

namespace ThingSet::Can {

StreamingCanThingSetBinaryEncoder::StreamingCanThingSetBinaryEncoder(Can::ThingSetCanServerTransport &transport)
    : StreamingThingSetBinaryEncoder::StreamingThingSetBinaryEncoder(), _transport(transport), _sequenceNumber(0)
{
    _buffer[0] = ThingSetRequestType::report;
    zcbor_new_encode_state(_state, BINARY_ENCODER_DEFAULT_MAX_DEPTH, &_buffer[1], _buffer.size() - 1, 1);
}

bool StreamingCanThingSetBinaryEncoder::send(Can::MultiFrameMessageType frameType, size_t length)
{
    Can::CanID id;
    id.setMessagePriority(Can::MessagePriority::reportLow)
        .setMessageType(Can::MessageType::multiFrameReport)
        .setMultiFrameMessageType(frameType)
        .setSequenceNumber(_sequenceNumber++);
    return _transport.publish(id, &_buffer[0], length);
}

bool StreamingCanThingSetBinaryEncoder::write(size_t length, bool flushing)
{
    return send(this->_exportedLength == 0
                    ? (flushing ? Can::MultiFrameMessageType::single : Can::MultiFrameMessageType::first)
                    : (flushing ? MultiFrameMessageType::last : Can::MultiFrameMessageType::consecutive),
                length);
}

} // namespace ThingSet::Can