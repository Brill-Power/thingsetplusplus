/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "thingset++/can/socketcan/StreamingCanThingSetBinaryDecoder.hpp"
#include "thingset++/ThingSetStatus.hpp"

namespace ThingSet::Can::SocketCan {

StreamingCanThingSetBinaryDecoder::StreamingCanThingSetBinaryDecoder() : StreamingThingSetBinaryDecoder(2)
{
}

int StreamingCanThingSetBinaryDecoder::read()
{
    return read(THINGSET_STREAMING_DECODER_CAN_MSG_SIZE, THINGSET_STREAMING_DECODER_CAN_MSG_SIZE);
}

bool StreamingCanThingSetBinaryDecoder::enqueue(CanFdFrame &&frame)
{
    _queue.push(std::move(frame));
    auto messageType = frame.getId().getMultiFrameMessageType();
    // for now, require reception of all frames before marking as ready
    // if we could make the pull parsing blocking, perhaps we can revise this
    if (messageType == MultiFrameMessageType::single || messageType == MultiFrameMessageType::last) {
        // fill buffer
        read(0, THINGSET_STREAMING_DECODER_CAN_MSG_SIZE * 2);
        return true;
    }
    return false;
}

int StreamingCanThingSetBinaryDecoder::read(size_t pos, size_t maxSize)
{
    if (_queue.empty()) {
        return 0;
    }
    while ((pos + THINGSET_STREAMING_DECODER_CAN_MSG_SIZE) <= maxSize) {
        CanFdFrame &frame = _queue.front();
        size_t offset = frame.getData()[0] == ThingSetRequestType::report ? 1: 0; // handle first frame
        size_t length = frame.getLength() - offset;
        memcpy(_buffer.data(), &frame.getData()[offset], length);
        pos += length;
        _queue.pop();
    }
    return pos;
}

} // ThingSet::Can::SocketCan