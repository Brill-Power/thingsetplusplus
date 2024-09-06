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

ThingSetCanInterface &ThingSetSocketCanServerTransport::getInterface() {
    return _canInterface;
}

} // namespace ThingSet::Can::SocketCan