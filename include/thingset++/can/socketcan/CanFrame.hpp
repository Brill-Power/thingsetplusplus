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

    SocketCanFrame(const CanID &id) : SocketCanFrame()
    {
        setId(id);
    }

    template <size_t BufferSize> requires (BufferSize <= Size)
    SocketCanFrame(const CanID &id, std::array<uint8_t, BufferSize> buffer) : SocketCanFrame(id)
    {
        memcpy(this->_frame.data, buffer.data(), buffer.size());
        setLength(buffer.size());
    }

public:
    CanID getId() const override
    {
        return CanID::create(this->_frame.can_id);
    }

    Self &setId(const CanID &id) override
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
    CanFrame(const CanID &id);
    template <size_t BufferSize> requires (BufferSize <= CAN_MAX_DLEN)
    CanFrame(const CanID &id, std::array<uint8_t, BufferSize> buffer) : SocketCanFrame(id, buffer)
    {}
};

class CanFdFrame : public SocketCanFrame<CanFdFrame, canfd_frame, CANFD_MAX_DLEN>
{
public:
    CanFdFrame();
    CanFdFrame(const CanID &id);
    template <size_t BufferSize> requires (BufferSize <= CANFD_MAX_DLEN)
    CanFdFrame(const CanID &id, std::array<uint8_t, BufferSize> buffer) : SocketCanFrame(id, buffer)
    {}
};

} // namespace ThingSet::Can::SocketCan