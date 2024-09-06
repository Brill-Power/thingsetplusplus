/*
 * Copyright (c) 2024 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */
#include "can/zephyr/CanFrame.hpp"
#include <string.h>

namespace ThingSet::Can::Zephyr {

using namespace ThingSet::Can;

CanFrame::CanFrame() : AbstractCanFrame<CanFrame, can_frame, CAN_MAX_DLEN>::AbstractCanFrame()
{}

CanFrame::CanFrame(CanID &id) : CanFrame()
{
    setId(id);
}

CanFrame::CanFrame(CanID &id, std::array<uint8_t, CAN_MAX_DLEN> buffer) : CanFrame(id)
{
    memcpy(_frame.data, buffer.data(), buffer.size());
    setLength(buffer.size());
}

CanID CanFrame::getId() const
{
    return CanID::create(_frame.id);
}

CanFrame &CanFrame::setId(CanID &id)
{
    _frame.id = id;
    if (_frame.id >= 0x800) {
        _frame.flags |= CAN_FRAME_IDE;
    }
    if (IS_ENABLED(CONFIG_CAN_FD_MODE)) {
        _frame.flags |= CAN_FRAME_FDF;
    }
    return *this;
}

uint8_t CanFrame::getLength() const
{
    return can_dlc_to_bytes(_frame.dlc);
}

CanFrame &CanFrame::setLength(uint8_t length)
{
    _frame.dlc = can_bytes_to_dlc(length);
    return *this;
}

} // namespace ThingSet::Can::Zephyr