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
    CanFrame(const CanID &id);

    template <size_t Size> requires (Size <= CAN_MAX_DLEN)
    CanFrame(const CanID &id, std::array<uint8_t, Size> buffer) : CanFrame(id)
    {
        memcpy(_frame.data, buffer.data(), buffer.size());
        setLength(buffer.size());
    }

    CanID getId() const override;
    CanFrame &setId(const CanID &id) override;
    uint8_t getLength() const override;
    CanFrame &setLength(const uint8_t length) override;
    bool getFd() const;
    CanFrame &setFd(bool value);
};

} // namespace ThingSet::Can::Zephyr