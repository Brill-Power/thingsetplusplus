/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "thingset++/can/socketcan/ThingSetSocketCanClientTransport.hpp"
#include "thingset++/ThingSetStatus.hpp"

namespace ThingSet::Can::SocketCan {

ThingSetSocketCanClientTransport::ThingSetSocketCanClientTransport(ThingSetSocketCanInterface &canInterface, const uint8_t targetNodeAddress) : ThingSetCanClientTransport(targetNodeAddress), _canInterface(canInterface)
{}

bool ThingSetSocketCanClientTransport::connect()
{
    auto rxId = CanID().setSource(_targetNodeAddress)
        .setTarget(_canInterface.getNodeAddress())
        .setMessageType(MessageType::requestResponse)
        .setMessagePriority(MessagePriority::channel);
    auto txId = rxId.getReplyId();
    _requestResponseSocket.setIsFd(true);
    return _requestResponseSocket.bind(_canInterface.getDeviceName(), rxId, txId);
}

int ThingSetSocketCanClientTransport::read(uint8_t *buffer, size_t len)
{
    return _requestResponseSocket.read(buffer, len);
}

bool ThingSetSocketCanClientTransport::write(uint8_t *buffer, size_t len)
{
    int written = _requestResponseSocket.write(buffer, len);
    return (written > 0 && written == (int)len);
}

} // ThingSet::Can::SocketCan