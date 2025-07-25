/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "thingset++/StreamingQueuingThingSetBinaryDecoder.hpp"
#include "thingset++/can/CanID.hpp"

#if defined(__ZEPHYR__)
#include "zephyr/drivers/can.h"
#define THINGSET_STREAMING_DECODER_CAN_MSG_SIZE CAN_MAX_DLEN
#elif defined(__linux__)
#include "linux/can.h"
// for now, on Linux, assume CAN FD
#define THINGSET_STREAMING_DECODER_CAN_MSG_SIZE CANFD_MAX_DLEN
#endif

namespace ThingSet::Can {

template <typename Frame>
class StreamingCanThingSetBinaryDecoder : public StreamingQueuingThingSetBinaryDecoder<THINGSET_STREAMING_DECODER_CAN_MSG_SIZE, Frame, MultiFrameMessageType>
{
public:
    StreamingCanThingSetBinaryDecoder() : StreamingQueuingThingSetBinaryDecoder<THINGSET_STREAMING_DECODER_CAN_MSG_SIZE, Frame, MultiFrameMessageType>()
    {}

protected:
    void getBuffer(const Frame &message, const uint8_t **buffer, size_t *length) const
    {
        *buffer = message.getData();
        *length = message.getLength();
    }
};

} // ThingSet::Can
