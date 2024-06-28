/*
 * Copyright (c) 2024 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */
#pragma once

#include "ThingSetBinaryDecoder.hpp"
#include "ThingSetBinaryEncoder.hpp"

namespace ThingSet {

/// @brief Represents a ThingSet value of a given type.
/// @tparam T The type of the value.
template <typename T> class ThingSetValue : public ThingSetBinaryEncodable, public ThingSetBinaryDecodable
{
protected:
    T _value;

public:
    ThingSetValue() : _value(T())
    {}
    ThingSetValue(const T &value) : _value(value)
    {}
    ThingSetValue(T &&value) : _value(std::move(value))
    {}
    template <class U, typename std::enable_if<std::is_convertible_v<U, T>, bool>::type = true>
    ThingSetValue(const U &value) : _value(value)
    {}

    bool encode(ThingSetBinaryEncoder &encoder) override
    {
        return encoder.encode(_value);
    }

    bool decode(ThingSetBinaryDecoder &decoder) override
    {
        return decoder.decode(&_value);
    }

    T &getValue()
    {
        return _value;
    }

    operator T &()
    {
        return _value;
    }

    T *operator&()
    {
        return &_value;
    }

    T &operator()()
    {
        return _value;
    }

    const T &operator()() const
    {
        return _value;
    }

    auto &operator=(const T &value)
    {
        _value = value;
        return *this;
    }

    auto &operator=(T &&value)
    {
        _value = std::move(value);
        return *this;
    }

    template <class U, typename std::enable_if<std::is_convertible_v<U, T>, bool>::type = true>
    auto &operator=(const U &value)
    {
        _value = value;
        return this;
    }
};

}; // namespace ThingSet
