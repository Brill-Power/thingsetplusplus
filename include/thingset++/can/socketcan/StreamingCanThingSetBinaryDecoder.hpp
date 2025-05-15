/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "thingset++/StreamingThingSetBinaryDecoder.hpp"
#include "thingset++/can/CanID.hpp"
#include "thingset++/can/socketcan/RawCanSocket.hpp"
#include <queue>
#include <atomic>

#if defined(__ZEPHYR__)
#include "zephyr/drivers/can.h"
#define THINGSET_STREAMING_DECODER_CAN_MSG_SIZE CAN_MAX_DLEN
#elif defined(__linux__)
#include "linux/can.h"
// for now, on Linux, assume CAN FD
#define THINGSET_STREAMING_DECODER_CAN_MSG_SIZE CANFD_MAX_DLEN
#endif

namespace ThingSet::Can::SocketCan {

class StreamingCanThingSetBinaryDecoder : public StreamingThingSetBinaryDecoder<THINGSET_STREAMING_DECODER_CAN_MSG_SIZE>
{
private:
    std::queue<CanFdFrame> _queue;

public:
    StreamingCanThingSetBinaryDecoder();

    bool enqueue(CanFdFrame &&frame);

protected:
    int read() override;

private:
    int read(size_t pos, size_t maxSize);
};

} // ThingSet::Can::SocketCan
