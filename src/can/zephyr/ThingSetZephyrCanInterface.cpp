/*
 * Copyright (c) 2024 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */
#include "can/zephyr/ThingSetZephyrCanInterface.hpp"
#include "can/zephyr/CanFrame.hpp"
#include "Eui.hpp"
#include "can/CanID.hpp"
#include "internal/logging.hpp"
#include <chrono>
#include <random>
#include <string.h>

namespace ThingSet::Can::Zephyr {

K_SEM_DEFINE(report_tx_sem, 0, 1);

static void streaming_encoder_can_send_report_tx_cb(const struct device *dev, int error,
                                                    void *user_data)
{
    ARG_UNUSED(user_data);
    k_sem_give(&report_tx_sem);
}

ThingSetZephyrCanInterface::ThingSetZephyrCanInterface(const device *const canDevice)
    : _canDevice(canDevice)
{}

ThingSetZephyrCanInterface::~ThingSetZephyrCanInterface()
{
}

bool ThingSetZephyrCanInterface::bind(uint8_t nodeAddress)
{
    // no-op for now; negotiation happens elsewhere
    _nodeAddress = nodeAddress;
    return true;
}

bool ThingSetZephyrCanInterface::publish(CanID &id, uint8_t *buffer, size_t length)
{
    CanFrame frame(id);
    memcpy(frame.getData(), buffer, length);
    frame.setLength(length);
    int result = can_send(_canDevice, frame.getFrame(), K_MSEC(CONFIG_THINGSET_CAN_REPORT_SEND_TIMEOUT), nullptr, nullptr);
    return result == 0;
}

bool ThingSetZephyrCanInterface::listen(std::function<int(uint8_t *, size_t, uint8_t *, size_t)> callback)
{
    return true;
}

} // namespace ThingSet::Can::Zephyr