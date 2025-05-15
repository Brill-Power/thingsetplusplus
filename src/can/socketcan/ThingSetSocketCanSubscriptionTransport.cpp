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

void ThingSetSocketCanSubscriptionTransport::subscribe(std::function<void(const CanID &, ThingSetBinaryDecoder &)> callback) {
    _listener.run(_canInterface.getDeviceName(), callback);
}

void ThingSetSocketCanSubscriptionTransport::SubscriptionListener::run(const std::string &deviceName, std::function<void(const CanID &, ThingSetBinaryDecoder &)> callback)
{
    LOG_DBG("Starting subscription listener");
    _socket.setIsFd(true);
    _socket.setFilter(CanID().setMessageType(MessageType::multiFrameReport)
        .setMessagePriority(MessagePriority::reportHigh));
    if (!_socket.bind(deviceName)) {
        LOG_ERR("Failed");
    }
    std::map<uint8_t, StreamingCanThingSetBinaryDecoder> decodersByNodeAddress;
    auto runner = [&]() {
        while (_run) {
            CanFdFrame frame;
            if (_socket.read(frame) > 0) {
            //if (_socket.tryRead(frame, std::chrono::milliseconds(500)) > 0) {
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
                    auto canId = frame.getId();
                    callback(canId, *decoder);
                }
            }
        }
    };
    _thread = std::thread(runner);
}

}