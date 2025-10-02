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

StreamingCanThingSetBinaryEncoder ThingSetCanServerTransport::getPublishingEncoder(bool enhanced)
{
    return StreamingCanThingSetBinaryEncoder(*this, enhanced);
}

bool ThingSetCanServerTransport::publish(CanID &id, uint8_t *buffer, size_t length)
{
    id.setSource(getInterface().getNodeAddress()).setMessageNumber(_messageNumber);
    bool result = doPublish(id, buffer, length);
    auto type = id.getMultiFrameMessageType();
    if (type == MultiFrameMessageType::last || type == MultiFrameMessageType::single) {
        _messageNumber++;
    }
    return result;
}

} // namespace ThingSet::Can