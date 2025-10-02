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

template <typename T>
concept EncodableNode = std::is_base_of_v<ThingSet::ThingSetNode, T> && std::is_base_of_v<ThingSet::ThingSetEncodable, T>;

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

    template <EncodableNode... Property>
    bool sendControl(Property &...properties)
    {
        static uint8_t buffer[CAN_MAX_DLEN];

        ([&]() {
            FixedDepthThingSetBinaryEncoder encoder(buffer, CAN_MAX_DLEN);

            if (!encoder.encode(properties)) {
                return false;
            }

            CanID canId = CanID()
                .setSource(getInterface().getNodeAddress())
                .setDataID(properties.getId())
                .setMessageType(MessageType::singleFrameReport)
                .setMessagePriority(MessagePriority::reportLow);

            if (!doPublish(canId, buffer, encoder.getEncodedLength())) {
                return false;;
            }
        }(), ...);

        return true;
    }
};

} // namespace ThingSet::Can