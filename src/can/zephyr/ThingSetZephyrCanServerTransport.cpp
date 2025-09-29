/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "thingset++/can/zephyr/ThingSetZephyrCanServerTransport.hpp"
#include "thingset++/can/zephyr/CanFrame.hpp"
#include "thingset++/ThingSetStatus.hpp"
#include "thingset++/internal/logging.hpp"

namespace ThingSet::Can::Zephyr {

ThingSetZephyrCanServerTransport::~ThingSetZephyrCanServerTransport()
{
}

ThingSetCanInterface &ThingSetZephyrCanServerTransport::getInterface()
{
    return _requestResponseContext.getInterface();
}

bool ThingSetZephyrCanServerTransport::doPublish(const Can::CanID &id, uint8_t *buffer, size_t length)
{
    CanFrame frame(id);
    if (length > CAN_MAX_DLEN) {
        // avoids a hard fault from the memcpy
        return false;
    }
    memcpy(frame.getData(), buffer, length);
    frame.setLength(length);
    frame.setFd(true);
    int result =
        can_send(_requestResponseContext.getInterface().getDevice(), frame.getFrame(), K_MSEC(CONFIG_THINGSET_PLUS_PLUS_CAN_REPORT_SEND_TIMEOUT), nullptr, nullptr);
    return result == 0;
}

bool ThingSetZephyrCanServerTransport::listen(std::function<int(const CanID &, uint8_t *, size_t, uint8_t *, size_t)> callback)
{
    return _requestResponseContext.bind(callback);
}

} // namespace ThingSet::Can::Zephyr