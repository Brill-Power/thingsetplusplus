/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "CanID.hpp"
#include <array>
#include <string.h>

namespace ThingSet::Can {

/// @brief Abstraction of a CAN frame across different platforms.
/// @tparam Self The derived type.
/// @tparam T The native type of the CAN frame on the platform.
/// @tparam Size The maximum data length.
template <typename Self, typename T, unsigned Size> class AbstractCanFrame
{
protected:
    T _frame;

    AbstractCanFrame() : _frame{}
    {}

public:
    static constexpr size_t size()
    {
        return sizeof(T);
    }

    T *getFrame()
    {
        return &_frame;
    }

    virtual CanID getId() const = 0;
    virtual Self &setId(const CanID &id) = 0;

    uint8_t *getData()
    {
        return _frame.data;
    }

    virtual uint8_t getLength() const = 0;
    virtual Self &setLength(uint8_t length) = 0;
};

} // namespace ThingSet::Can