/*
 * Copyright (c) 2024 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */
#pragma once

#include "can/AbstractCanFrame.hpp"
#include "linux/can.h"
#include "linux/can/raw.h"
#include <array>

namespace ThingSet::Can::SocketCan {

template <typename Self, typename T, unsigned Size>
class SocketCanFrame : public AbstractCanFrame<Self, T, Size>
{
    SocketCanFrame(){}
    SocketCanFrame(CanID id)
    {
        setId(id);
    }
    SocketCanFrame(CanID id, std::array<uint8_t, CAN_MAX_DLEN> buffer): SocketCanFrame(id)
    {
        memcpy(_frame.data, buffer.data(), buffer.size());
        setLength(buffer.size());
    }

    CanID getId() const override
    {
        return CanID::create(_frame.can_id);
    }

    Self &setId(CanID id) override
    {
        _frame.can_id = id;
        if (_frame.can_id >= 0x800) {
            _frame.can_id |= CAN_EFF_FLAG;
        }
        return static_cast<Self &>(*this);
    }

    uint8_t getLength() const override
    {
        return _frame.len;
    }

    Self &setLength(uint8_t length) override
    {
        _frame.len = length;
        return static_cast<Self &>(*this);
    }
};

class CanFrame : public SocketCanFrame<CanFrame, can_frame, CAN_MAX_DLEN>
{
public:
    CanFrame();
    CanFrame(CanID id);
    CanFrame(CanID id, std::array<uint8_t, CAN_MAX_DLEN> buffer);
};

class CanFdFrame : public SocketCanFrame<CanFdFrame, canfd_frame, CANFD_MAX_DLEN>
{
public:
    CanFdFrame();
    CanFdFrame(CanID id);
    CanFdFrame(CanID id, std::array<uint8_t, CANFD_MAX_DLEN> buffer);
};

} // namespace ThingSet::Can::SocketCan