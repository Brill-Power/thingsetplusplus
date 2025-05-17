/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "thingset++/can/socketcan/ThingSetSocketCanSubscriptionTransport.hpp"
#include "thingset++/can/socketcan/StreamingCanThingSetBinaryDecoder.hpp"
#include "thingset++/internal/logging.hpp"
#include "thingset++/ThingSetStatus.hpp"

namespace ThingSet::Can::SocketCan {

ThingSetSocketCanSubscriptionTransport::ThingSetSocketCanSubscriptionTransport(ThingSetSocketCanInterface &canInterface) : _canInterface(canInterface)
{}

ThingSetCanInterface &ThingSetSocketCanSubscriptionTransport::getInterface() {
    return _canInterface;
}

bool ThingSetSocketCanSubscriptionTransport::subscribe(std::function<void(const CanID &, ThingSetBinaryDecoder &)> callback) {
    return _listener.run(_canInterface.getDeviceName(), callback);
}

bool ThingSetSocketCanSubscriptionTransport::SubscriptionListener::run(const std::string &deviceName, std::function<void(const CanID &, ThingSetBinaryDecoder &)> callback)
{
    _socket.setIsFd(true);
    auto filter = CanID().setMessageType(MessageType::multiFrameReport)
        .setMessagePriority(MessagePriority::reportHigh)
        .setMask(CanID().setMessageType(MessageType::multiFrameReport).getMask() | MessagePriority::reportHigh); // override calculated mask
    _socket.setFilter(filter);
    if (!_socket.bind(deviceName)) {
        // throw?
        return false;
    }
    auto runner = [&]() {
        std::map<uint8_t, StreamingCanThingSetBinaryDecoder> decodersByNodeAddress;
        while (_run) {
            CanFdFrame frame;
            if (_socket.read(frame) > 0) {
                uint8_t sender = frame.getId().getSource();
                StreamingCanThingSetBinaryDecoder *decoder = nullptr;
                if (frame.getData()[0] == ThingSetRequestType::report) {
                    // new report, so create a new decoder and add to map
                    decodersByNodeAddress[sender] = StreamingCanThingSetBinaryDecoder();
                    decoder = &decodersByNodeAddress[sender];
                } else {
                    auto found = decodersByNodeAddress.find(sender);
                    if (found != decodersByNodeAddress.end()) {
                        decoder = &found->second;
                    }
                }
                if (decoder && decoder->enqueue(std::move(frame))) {
                    callback(frame.getId(), *decoder);
                }
            }
        }
    };
    _thread = std::thread(runner);
    _thread.join();
    return true;
}

}