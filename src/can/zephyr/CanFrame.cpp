/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "thingset++/can/zephyr/CanFrame.hpp"
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

bool CanFrame::getFd() const
{
    return (_frame.flags & CAN_FRAME_FDF);
}

CanFrame &CanFrame::setFd(bool value)
{
    if (IS_ENABLED(CONFIG_CAN_FD_MODE)) {
        if (value) {
            _frame.flags |= CAN_FRAME_FDF;
        }
        else {
            _frame.flags &= ~CAN_FRAME_FDF;
        }
    }
    return *this;
}

CanFrame &CanFrame::setId(CanID &id)
{
    _frame.id = id;
    if (_frame.id >= 0x800) {
        _frame.flags |= CAN_FRAME_IDE;
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