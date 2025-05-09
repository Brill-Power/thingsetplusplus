/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "thingset++/can/ThingSetCanServerTransport.hpp"

namespace ThingSet::Can {

ThingSetCanServerTransport::ThingSetCanServerTransport() : _messageNumber(0)
{}

uint8_t ThingSetCanServerTransport::getNodeAddress()
{
    return getInterface().getNodeAddress();
}

bool ThingSetCanServerTransport::listen(std::function<int(CanID &, uint8_t *, size_t, uint8_t *, size_t)> callback)
{
    return getInterface().bind() && getInterface().listen(callback);
}

bool ThingSetCanServerTransport::publish(CanID &id, uint8_t *buffer, size_t length)
{
    id.setSource(getInterface().getNodeAddress()).setMessageNumber(_messageNumber);
    bool result = getInterface().publish(id, buffer, length);
    auto type = id.getMultiFrameMessageType();
    if (type == MultiFrameMessageType::last || type == MultiFrameMessageType::single) {
        _messageNumber++;
    }
    return result;
}

bool ThingSetCanServerTransport::publish(uint8_t *buffer, size_t length)
{
    // not a lot we can do with this code path: we would need the object ID
    // to send as a control message; so we just assume it's a multi-frame
    // report that fits in a single frame
    return publish(CanID()
                       .setSequenceNumber(0)
                       .setMessageType(MessageType::multiFrameReport)
                       .setMultiFrameMessageType(MultiFrameMessageType::single)
                       .setMessagePriority(MessagePriority::reportLow),
                   buffer, length);
}

} // namespace ThingSet::Can