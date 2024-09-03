/*
 * Copyright (c) 2024 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */
#pragma once

#include "CanID.hpp"
#include "linux/can.h"
#include "linux/can/raw.h"
#include <array>
#include <string.h>

namespace ThingSet::Can {

template <typename Self, typename T, unsigned Size> class AbstractCanFrame
{
protected:
    T _frame;

protected:
    AbstractCanFrame()
    {}

    AbstractCanFrame(CanID id)
    {
        setId(id);
    }

    AbstractCanFrame(CanID id, std::array<uint8_t, Size> buffer) : AbstractCanFrame(id)
    {
        memcpy(_frame.data, buffer.data(), buffer.size());
        setLength(buffer.size());
    }

public:
    CanID getId() const
    {
        return CanID::create(_frame.can_id);
    }

    static constexpr size_t size()
    {
        return sizeof(T);
    }

    Self &setId(CanID id)
    {
        _frame.can_id = id;
        if (_frame.can_id >= 0x800) {
            _frame.can_id |= CAN_EFF_FLAG;
        }
        return static_cast<Self &>(*this);
    }

    uint8_t *getData()
    {
        return _frame.data;
    }

    uint8_t getLength() const
    {
        return _frame.len;
    }

    Self &setLength(uint8_t length)
    {
        _frame.len = length;
        return static_cast<Self &>(*this);
    }
};

} // namespace ThingSet::Can