/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "thingset++/ThingSetServerTransport.hpp"
#include "thingset++/can/CanID.hpp"
#include "thingset++/can/ThingSetCanInterface.hpp"
#include "thingset++/can/StreamingCanThingSetBinaryEncoder.hpp"

namespace ThingSet::Can {

class ThingSetCanServerTransport : public ThingSetServerTransport<CanID, THINGSET_STREAMING_ENCODER_CAN_MSG_SIZE, StreamingCanThingSetBinaryEncoder>
{
protected:
    uint8_t _messageNumber;

    ThingSetCanServerTransport();

    virtual ThingSetCanInterface &getInterface() = 0;

    virtual bool doPublish(const Can::CanID &id, uint8_t *buffer, size_t length) = 0;

public:
    uint8_t getNodeAddress();

    StreamingCanThingSetBinaryEncoder getPublishingEncoder() override;

    bool publish(Can::CanID &id, uint8_t *buffer, size_t length);

    template <typename T>
    bool sendControl(uint16_t id, T value)
    {
        static CanID canId = CanID()
            .setSource(getInterface().getNodeAddress())
            .setDataID(id)
            .setMessageType(MessageType::singleFrameReport)
            .setMessagePriority(MessagePriority::reportLow);

        static uint8_t buffer[CAN_MAX_DLEN];

        FixedDepthThingSetBinaryEncoder encoder(buffer, CAN_MAX_DLEN);
        encoder.encode(value);

        return doPublish(canId, buffer, encoder.getEncodedLength());
    }
};

} // namespace ThingSet::Can