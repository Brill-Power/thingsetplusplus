/*
 * Copyright (c) 2024 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */
#pragma once

#include "ThingSetBinaryEncoder.hpp"
#include "can/CanID.hpp"
#include "can/ThingSetCanServerTransport.hpp"
#include <array>
#include <zcbor_encode.h>

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
class StreamingThingSetBinaryEncoder : public ThingSetBinaryEncoder
{
private:
    zcbor_state_t _state[BINARY_ENCODER_DEFAULT_MAX_DEPTH];
    ThingSetCanServerTransport &_transport;
    std::array<uint8_t, THINGSET_STREAMING_ENCODER_CAN_MSG_SIZE * 2> _buffer;
    uint8_t _sequenceNumber;
    size_t _exportedLength;

public:
    StreamingThingSetBinaryEncoder(ThingSetCanServerTransport &transport);

    size_t getEncodedLength() const override;

    const bool getIsForwardOnly() const override;

    template <typename T> bool encodePair(uint32_t id, T &value)
    {
        return encode(id) && encode(value);
    }

    /// @brief Write remaining buffer to a final CAN message.
    void flush();

protected:
    zcbor_state_t *getState() override;

private:
    bool ensureLength();
    void send(Can::MultiFrameMessageType frameType, size_t length);
};

} // namespace ThingSet::Can
