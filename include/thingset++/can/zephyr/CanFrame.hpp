/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "thingset++/can/AbstractCanFrame.hpp"
#include "zephyr/drivers/can.h"
#include <array>

namespace ThingSet::Can::Zephyr {

class CanFrame : public AbstractCanFrame<CanFrame, can_frame, CAN_MAX_DLEN>
{
public:
    CanFrame();
    CanFrame(CanID &id);
    CanFrame(CanID &id, std::array<uint8_t, CAN_MAX_DLEN> buffer);

    CanID getId() const override;
    CanFrame &setId(CanID &id) override;
    uint8_t getLength() const override;
    CanFrame &setLength(uint8_t length) override;
    bool getFd() const;
    CanFrame &setFd(bool value);
};

} // namespace ThingSet::Can::Zephyr