/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "ThingSetCanServerTransport.hpp"
#include "thingset++/StreamingThingSetBinaryEncoder.hpp"

#if defined(__ZEPHYR__)
#include "zephyr/drivers/can.h"
#define THINGSET_STREAMING_ENCODER_CAN_MSG_SIZE CAN_MAX_DLEN
#elif defined(__linux__)
#include "linux/can.h"
// for now, on Linux, assume CAN FD
#define THINGSET_STREAMING_ENCODER_CAN_MSG_SIZE CANFD_MAX_DLEN
#endif

namespace ThingSet::Can {

/// @brief Encodes data to successive CAN messages in a multi-frame report.
class StreamingCanThingSetBinaryEncoder : public StreamingThingSetBinaryEncoder<THINGSET_STREAMING_ENCODER_CAN_MSG_SIZE>
{
private:
    ThingSetCanServerTransport &_transport;
    uint8_t _sequenceNumber;

public:
    StreamingCanThingSetBinaryEncoder(ThingSetCanServerTransport &transport);

protected:
    bool write(size_t length, bool flushing) override;

private:
    bool send(Can::MultiFrameMessageType frameType, size_t length);
};

} // namespace ThingSet::Can