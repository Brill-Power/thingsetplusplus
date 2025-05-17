/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "thingset++/can/zephyr/ThingSetZephyrCanServerTransport.hpp"

namespace ThingSet::Can::Zephyr {

ThingSetZephyrCanServerTransport::ThingSetZephyrCanServerTransport(_ThingSetZephyrCanInterface &canInterface)
    : ThingSetCanServerTransport(), _canInterface(canInterface)
{}

ThingSetCanInterface &ThingSetZephyrCanServerTransport::getInterface()
{
    return _canInterface;
}

bool ThingSetZephyrCanServerTransport::doPublish(Can::CanID &id, uint8_t *buffer, size_t length)
{
    CanFrame frame(id);
    memcpy(frame.getData(), buffer, length);
    frame.setLength(length);
    frame.setFd(true);
    int result =
        can_send(_canInterface.getDevice(), frame.getFrame(), K_MSEC(CONFIG_THINGSET_CAN_REPORT_SEND_TIMEOUT), nullptr, nullptr);
    return result == 0;
}

} // namespace ThingSet::Can::Zephyr