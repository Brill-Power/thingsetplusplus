/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "thingset++/can/ThingSetCanSubscriptionTransport.hpp"

namespace ThingSet::Can {

const CanID _ThingSetCanSubscriptionTransport::reportFilter = CanID().setMessageType(MessageType::multiFrameReport)
    .setMessagePriority(MessagePriority::reportHigh)
    .setMask(CanID().setMessageType(MessageType::multiFrameReport).getMask() | MessagePriority::reportHigh); // override calculated mask

const CanID _ThingSetCanSubscriptionTransport::controlFilter = CanID().setMessageType(MessageType::singleFrameReport)
    .setMessagePriority(MessagePriority::reportLow);

}