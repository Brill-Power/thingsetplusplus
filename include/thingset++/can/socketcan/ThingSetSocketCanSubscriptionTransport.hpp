/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "thingset++/can/ThingSetCanSubscriptionTransport.hpp"
#include "thingset++/can/socketcan/ThingSetSocketCanInterface.hpp"

namespace ThingSet::Can::SocketCan {

class ThingSetSocketCanSubscriptionTransport : public ThingSetCanSubscriptionTransport<CanFdFrame>
{
private:
    class SocketCanSubscriptionListener : protected SubscriptionListener, public RawCanSocketListener
    {
    public:
        bool run(const std::string &deviceName, std::function<void(const CanID &, ThingSetBinaryDecoder &)> callback);
    };

    ThingSetSocketCanInterface &_canInterface;
    SocketCanSubscriptionListener _listener;

public:
    ThingSetSocketCanSubscriptionTransport(ThingSetSocketCanInterface &canInterface);

    bool subscribe(std::function<void(const CanID &, ThingSetBinaryDecoder &)> callback) override;

protected:
    ThingSetCanInterface &getInterface() override;
};

} // namespace ThingSet::Can::SocketCan