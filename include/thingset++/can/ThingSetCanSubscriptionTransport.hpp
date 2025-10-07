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
    return message.getId().getMultiFrameMessageType();
}

class _ThingSetCanSubscriptionTransport
{
protected:
    static const CanID reportFilter;
    static const CanID controlFilter;

protected:
    virtual ThingSetCanInterface &getInterface() = 0;
};

template <typename Frame>
class ThingSetCanSubscriptionTransport : public _ThingSetCanSubscriptionTransport, public ThingSetMultiFrameSubscriptionTransport<CanID>
{
};

template <typename Frame>
class ThingSetCanControlSubscriptionTransport : public _ThingSetCanSubscriptionTransport, public ThingSetSubscriptionTransport<CanID>
{
};

} // namespace ThingSet::Can