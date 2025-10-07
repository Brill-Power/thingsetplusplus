/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "thingset++/ThingSetSubscriptionTransport.hpp"
#include "thingset++/can/ThingSetCanInterface.hpp"
#include "thingset++/can/StreamingCanThingSetBinaryDecoder.hpp"
#include "thingset++/ThingSetStatus.hpp"
#include "thingset++/can/AbstractCanFrame.hpp"

namespace ThingSet::Can {

template <typename Frame, typename T = Frame::native_type, unsigned Size = Frame::payloadSize()>
    requires std::is_base_of_v<AbstractCanFrame<Frame, T, Size>, Frame>
MultiFrameMessageType getMessageType(const Frame &message)
{
    if (message.getId().getMessageType() == MessageType::singleFrameReport) {
        return MultiFrameMessageType::single;
    }
    return message.getId().getMultiFrameMessageType();
}

class _ThingSetCanSubscriptionTransport
{
protected:
    static const CanID subscriptionFilter;
    static const CanID singleFrameReportFilter;
};

template <typename Frame>
class ThingSetCanSubscriptionTransport : public _ThingSetCanSubscriptionTransport, public ThingSetSubscriptionTransport<CanID>
{
protected:
    virtual ThingSetCanInterface &getInterface() = 0;
};

} // namespace ThingSet::Can