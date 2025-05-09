/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "thingset++/can/ThingSetCanServerTransport.hpp"
#include "thingset++/can/socketcan/ThingSetSocketCanInterface.hpp"

namespace ThingSet::Can::SocketCan {

class ThingSetSocketCanServerTransport : public Can::ThingSetCanServerTransport
{
private:
    ThingSetSocketCanInterface _canInterface;

protected:
    ThingSetCanInterface &getInterface() override;

public:
    ThingSetSocketCanServerTransport(const std::string &deviceName);
};

} // namespace ThingSet::Can::SocketCan