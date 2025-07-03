/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "thingset++/can/socketcan/ThingSetSocketCanServerTransport.hpp"

namespace ThingSet::Can::SocketCan {

#define THINGSET_REQUEST_BUFFER_SIZE  1024
#define THINGSET_RESPONSE_BUFFER_SIZE 1024

ThingSetSocketCanServerTransport::ThingSetSocketCanServerTransport(ThingSetSocketCanInterface &canInterface)
    : ThingSetCanServerTransport(), _canInterface(canInterface), _listener(canInterface.getDeviceName(), true)
{
    _publishSocket.bind(canInterface.getDeviceName());
    _publishSocket.setIsFd(true);
}

ThingSetCanInterface &ThingSetSocketCanServerTransport::getInterface()
{
    return _canInterface;
}

bool ThingSetSocketCanServerTransport::doPublish(const Can::CanID &id, uint8_t *buffer, size_t length)
{
    CanFdFrame frame(id);
    if (length > CAN_MAX_DLEN) {
        // avoids a hard fault from the memcpy
        return false;
    }
    memcpy(frame.getData(), buffer, length);
    frame.setLength(length);
    return _publishSocket.write(frame) > 0;
}

bool ThingSetSocketCanServerTransport::listen(
    std::function<int(const CanID &, uint8_t *, size_t, uint8_t *, size_t)> callback)
{
    _listener.listen(CanID()
                         .setMessageType(MessageType::requestResponse)
                         .setMessagePriority(MessagePriority::channel)
                         .setBridge(CanID::defaultBridge)
                         .setTarget(_canInterface.getNodeAddress()),
                     [&](auto sender, auto socket) {
                         uint8_t request[THINGSET_REQUEST_BUFFER_SIZE];
                         int size = socket.read(request, sizeof(request));
                         printf("Got request of size %d bytes from 0x%x\n", size, sender.getId());
                         uint8_t response[THINGSET_RESPONSE_BUFFER_SIZE];
                         int responseLength = callback(sender, request, size, response, sizeof(response));
                         socket.write(response, responseLength);
                         printf("Sent response of size %d bytes to 0x%x\n", responseLength,
                                sender.getReplyId().getId());
                     });
    return true;
}

} // namespace ThingSet::Can::SocketCan