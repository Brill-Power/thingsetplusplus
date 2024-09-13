/*
 * Copyright (c) 2024 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */
#include "can/StreamingThingSetBinaryEncoder.hpp"
#include "ThingSetStatus.hpp"
#include <list>

namespace ThingSet::Can {

StreamingThingSetBinaryEncoder::StreamingThingSetBinaryEncoder(Can::ThingSetCanServerTransport &transport)
    : _transport(transport), _sequenceNumber(0), _exportedLength(0)
{
    _buffer[0] = ThingSetRequestType::report;
    zcbor_new_encode_state(_state, BINARY_ENCODER_DEFAULT_MAX_DEPTH, &_buffer[1],
                           (THINGSET_STREAMING_ENCODER_CAN_MSG_SIZE * 2) - 1, 1);
}

bool StreamingThingSetBinaryEncoder::getIsForwardOnly() const
{
    return true;
}

size_t StreamingThingSetBinaryEncoder::getEncodedLength() const
{
    return _exportedLength;
}

zcbor_state_t *StreamingThingSetBinaryEncoder::getState()
{
    ensureLength();
    return _state;
}

bool StreamingThingSetBinaryEncoder::ensureLength()
{
    size_t currentPos = _state->payload - &_buffer[0];
    if (currentPos > THINGSET_STREAMING_ENCODER_CAN_MSG_SIZE) {
        send(_exportedLength == 0 ? Can::MultiFrameMessageType::first : Can::MultiFrameMessageType::consecutive,
             THINGSET_STREAMING_ENCODER_CAN_MSG_SIZE);
        memmove(&_buffer[0], &_buffer[THINGSET_STREAMING_ENCODER_CAN_MSG_SIZE],
                THINGSET_STREAMING_ENCODER_CAN_MSG_SIZE);           // move to start
        _exportedLength += THINGSET_STREAMING_ENCODER_CAN_MSG_SIZE; // keep track
        size_t newPos = currentPos - THINGSET_STREAMING_ENCODER_CAN_MSG_SIZE;
        zcbor_update_state(_state, &_buffer[newPos], _buffer.size() - newPos);
    }
    return true;
}

void StreamingThingSetBinaryEncoder::send(Can::MultiFrameMessageType frameType, size_t length)
{
    Can::CanID id;
    id.setMessagePriority(Can::MessagePriority::reportLow)
        .setMessageType(Can::MessageType::multiFrameReport)
        .setMultiFrameMessageType(frameType)
        .setSequenceNumber(_sequenceNumber++);
    _transport.publish(id, &_buffer[0], length);
}

void StreamingThingSetBinaryEncoder::flush()
{
    size_t currentLength = _state->payload - &_buffer[0];
    size_t remainder = _buffer.size() - currentLength;
    memset(&_buffer[currentLength], 0, remainder);
    send(_exportedLength == 0 ? Can::MultiFrameMessageType::single : Can::MultiFrameMessageType::last, currentLength);
    _exportedLength += currentLength;
}

} // namespace ThingSet::Can
