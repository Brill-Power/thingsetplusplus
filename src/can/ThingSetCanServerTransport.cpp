/*
 * Copyright (c) 2024 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */
#include "can/ThingSetCanServerTransport.hpp"

namespace ThingSet::Can {

ThingSetCanServerTransport::ThingSetCanServerTransport() : _messageNumber(0)
{}

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