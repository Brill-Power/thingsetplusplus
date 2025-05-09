/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "thingset++/can/socketcan/ThingSetSocketCanServerTransport.hpp"

namespace ThingSet::Can::SocketCan {

ThingSetSocketCanServerTransport::ThingSetSocketCanServerTransport(const std::string &deviceName)
    : ThingSetCanServerTransport(), _canInterface(deviceName)
{}

ThingSetCanInterface &ThingSetSocketCanServerTransport::getInterface()
{
    return _canInterface;
}

} // namespace ThingSet::Can::SocketCan