/*
 * Copyright (c) 2024 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */
#include "can/socketcan/ThingSetSocketCanServerTransport.hpp"

namespace ThingSet::Can::SocketCan {

ThingSetSocketCanServerTransport::ThingSetSocketCanServerTransport(const std::string &deviceName)
    : ThingSetCanServerTransport(), _canInterface(deviceName)
{}

bool ThingSetSocketCanServerTransport::listen(std::function<int(uint8_t *, size_t, uint8_t *, size_t)> callback)
{
    return _canInterface.bind() && _canInterface.listen(callback);
}

bool ThingSetSocketCanServerTransport::publish(CanID &id, uint8_t *buffer, size_t length)
{
    id.setSource(_canInterface.getNodeAddress()).setMessageNumber(_messageNumber);
    bool result = _canInterface.publish(id, buffer, length);
    auto type = id.getMultiFrameMessageType();
    if (type == MultiFrameMessageType::last || type == MultiFrameMessageType::single) {
        _messageNumber++;
    }
    return result;
}

} // namespace ThingSet::Can::SocketCan