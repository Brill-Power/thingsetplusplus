/*
 * Copyright (c) 2024 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */
#include "can/zephyr/ThingSetZephyrCanServerTransport.hpp"

namespace ThingSet::Can::Zephyr {

ThingSetZephyrCanServerTransport::ThingSetZephyrCanServerTransport(const device *const canDevice)
    : ThingSetCanServerTransport(), _canInterface(canDevice)
{}

ThingSetCanInterface &ThingSetZephyrCanServerTransport::getInterface() {
    return _canInterface;
}

} // namespace ThingSet::Can::SocketCan