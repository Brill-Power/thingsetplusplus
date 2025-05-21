/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "thingset++/can/socketcan/ThingSetSocketCanSubscriptionTransport.hpp"
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

bool ThingSetSocketCanSubscriptionTransport::SocketCanSubscriptionListener::run(const std::string &deviceName, std::function<void(const CanID &, ThingSetBinaryDecoder &)> callback)
{
    _socket.setIsFd(true);
    _socket.setFilter(subscriptionFilter);
    if (!_socket.bind(deviceName)) {
        // throw?
        return false;
    }
    auto runner = [&]() {
        std::map<uint8_t, StreamingCanThingSetBinaryDecoder<CanFdFrame>> decodersByNodeAddress;
        while (_run) {
            CanFdFrame frame;
            if (_socket.read(frame) > 0) {
                handle(frame, decodersByNodeAddress, callback);
            }
        }
    };
    _thread = std::thread(runner);
    _thread.join();
    return true;
}

}