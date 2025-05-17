/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "thingset++/can/ThingSetCanClientTransport.hpp"
#include "thingset++/can/socketcan/ThingSetSocketCanInterface.hpp"

namespace ThingSet::Can::SocketCan {

class ThingSetSocketCanClientTransport : public ThingSetCanClientTransport
{
private:
    ThingSetSocketCanInterface &_canInterface;
    IsoTpCanSocket _requestResponseSocket;

public:
    ThingSetSocketCanClientTransport(ThingSetSocketCanInterface &canInterface, const uint8_t targetNodeAddress);

    bool connect() override;

protected:
    int read(uint8_t *buffer, size_t len) override;
    bool write(uint8_t *buffer, size_t len) override;
};

} // namespace ThingSet::Can::SocketCan