/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "thingset++/can/AbstractCanFrame.hpp"
#include "linux/can.h"
#include "linux/can/raw.h"
#include <array>

namespace ThingSet::Can::SocketCan {

template <typename Self, typename T, unsigned Size> class SocketCanFrame : public AbstractCanFrame<Self, T, Size>
{
protected:
    SocketCanFrame() : AbstractCanFrame<Self, T, Size>::AbstractCanFrame()
    {}

    SocketCanFrame(CanID &id) : SocketCanFrame()
    {
        setId(id);
    }

    SocketCanFrame(CanID &id, std::array<uint8_t, Size> buffer) : SocketCanFrame(id)
    {
        memcpy(this->_frame.data, buffer.data(), buffer.size());
        setLength(buffer.size());
    }

public:
    CanID getId() const override
    {
        return CanID::create(this->_frame.can_id);
    }

    Self &setId(CanID &id) override
    {
        this->_frame.can_id = id;
        if (this->_frame.can_id >= 0x800) {
            this->_frame.can_id |= CAN_EFF_FLAG;
        }
        return static_cast<Self &>(*this);
    }

    uint8_t getLength() const override
    {
        return this->_frame.len;
    }

    Self &setLength(uint8_t length) override
    {
        this->_frame.len = length;
        return static_cast<Self &>(*this);
    }
};

class CanFrame : public SocketCanFrame<CanFrame, can_frame, CAN_MAX_DLEN>
{
public:
    CanFrame();
    CanFrame(CanID &id);
    CanFrame(CanID &id, std::array<uint8_t, CAN_MAX_DLEN> buffer);
};

class CanFdFrame : public SocketCanFrame<CanFdFrame, canfd_frame, CANFD_MAX_DLEN>
{
public:
    CanFdFrame();
    CanFdFrame(CanID &id);
    CanFdFrame(CanID &id, std::array<uint8_t, CANFD_MAX_DLEN> buffer);
};

} // namespace ThingSet::Can::SocketCan