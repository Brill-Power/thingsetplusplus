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
    MultiFrameMessageType messageType = frame.getId().getMultiFrameMessageType();
    // for now, require reception of all frames before marking as ready
    // if we could make the pull parsing blocking, perhaps we can revise this
    if (messageType == MultiFrameMessageType::single || messageType == MultiFrameMessageType::last) {
        // fill buffer
        read(0, -1 + THINGSET_STREAMING_DECODER_CAN_MSG_SIZE * 2);
        zcbor_new_decode_state(_state, BINARY_DECODER_DEFAULT_MAX_DEPTH, &_buffer[1], _buffer.size() - 1, 2, NULL, 0);
        return true;
    }
    return false;
}

int StreamingCanThingSetBinaryDecoder::read(size_t pos, size_t maxSize)
{
    if (_queue.empty()) {
        return 0;
    }
    while (pos <= maxSize) {
        CanFdFrame &frame = _queue.front();
        memcpy(&_buffer.data()[pos], frame.getData(), frame.getLength());
        pos += frame.getLength();
        _queue.pop();
    }
    return pos;
}

} // ThingSet::Can::SocketCan