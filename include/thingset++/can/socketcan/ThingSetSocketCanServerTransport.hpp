/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "thingset++/can/ThingSetCanServerTransport.hpp"
#include "thingset++/can/socketcan/ThingSetSocketCanInterface.hpp"

namespace ThingSet::Can::SocketCan {

class ThingSetSocketCanServerTransport : public ThingSetCanServerTransport
{
private:
    ThingSetSocketCanInterface &_canInterface;
    IsoTpCanSocket::Listener _listener;
    RawCanSocket _publishSocket;

public:
    ThingSetSocketCanServerTransport(ThingSetSocketCanInterface &canInterface);

    bool listen(std::function<int(const CanID &, uint8_t *, size_t, uint8_t *, size_t)> callback) override;

protected:
    ThingSetCanInterface &getInterface() override;

    bool doPublish(const Can::CanID &id, uint8_t *buffer, size_t length) override;
};

} // namespace ThingSet::Can::SocketCan