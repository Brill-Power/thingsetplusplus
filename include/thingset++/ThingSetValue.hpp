/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "thingset++/ThingSetBinaryDecoder.hpp"
#include "thingset++/ThingSetBinaryEncoder.hpp"
#include "thingset++/ThingSetTextDecoder.hpp"
#include "thingset++/ThingSetTextEncoder.hpp"

namespace ThingSet {

/// @brief Represents a ThingSet value of a given type.
/// @tparam T The type of the value.
template <typename T>
class ThingSetValue : public ThingSetEncodable,
                      public ThingSetDecodable
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

    bool encode(ThingSetEncoder &encoder) const override
    {
        return encoder.encode(_value);
    }

    bool decode(ThingSetDecoder &decoder) override
    {
        if constexpr (std::is_const_v<T>) {
            return false;
        } else {
            return decoder.decode(&_value);
        }
    }

    T &getValue()
    {
        return _value;
    }

    const T &getValue() const
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
        return *this;
    }
};

template <typename Element, std::size_t Size>
class ThingSetValue<std::array<Element, Size>>
    : public ThingSetEncodable, public ThingSetDecodable
{
protected:
    std::array<Element, Size> _value;

public:
    ThingSetValue() : _value(std::array<Element, Size>())
    {}
    ThingSetValue(const std::array<Element, Size> &value) : _value(value)
    {}
    ThingSetValue(std::array<Element, Size> &&value) : _value(std::move(value))
    {}

    bool encode(ThingSetEncoder &encoder) const override
    {
        return encoder.encode(_value);
    }

    bool decode(ThingSetDecoder &decoder) override
    {
        return decoder.decode(&_value);
    }

    std::array<Element, Size> &getValue()
    {
        return _value;
    }

    const std::array<Element, Size> &getValue() const
    {
        return _value;
    }

    Element &operator[](int index)
    {
        return _value[index];
    }

    auto &operator=(const std::array<Element, Size> &value)
    {
        _value = value;
        return *this;
    }

    auto &operator=(std::array<Element, Size> &&value)
    {
        _value = std::move(value);
        return *this;
    }

    std::size_t size() const
    {
        return _value.size();
    }
};

template <typename T>
class ThingSetValue<T *>
    : public ThingSetEncodable, public ThingSetDecodable
{
protected:
    T *_value;

public:
    ThingSetValue(T *value) : _value(value)
    {}

    bool encode(ThingSetEncoder &encoder) const override
    {
        return encoder.encode(_value);
    }

    bool decode(ThingSetDecoder &decoder) override
    {
        if constexpr (std::is_const_v<T>) {
            return false;
        } else {
            return decoder.decode(_value);
        }
    }

    template <typename X> requires (!std::is_const_v<T>)
    auto &operator=(const T &value)
    {
        *_value = value;
        return *this;
    }

    template <typename X> requires (!std::is_const_v<T>)
    auto &operator=(T &&value)
    {
        *_value = std::move(value);
        return *this;
    }

    T *getValue()
    {
        return _value;
    }

    const T *getValue() const
    {
        return _value;
    }
};

template <typename Element, size_t Size>
class ThingSetValue<Element[Size]>
    : public ThingSetEncodable, public ThingSetDecodable
{
protected:
    Element _value[Size];

public:
    ThingSetValue(const Element value[Size])
    {
        memcpy(_value, value, Size * sizeof(Element));
    }

    bool encode(ThingSetEncoder &encoder) const override
    {
        return encoder.encode(_value, Size);
    }

    bool decode(ThingSetDecoder &decoder) override
    {
        if constexpr (std::is_const_v<Element>) {
            return false;
        } else {
            return decoder.decode(_value, Size);
        }
    }

    Element &operator[](int index)
    {
        // TODO: can we do a bounds check here and...do something if out of bounds?
        return _value[index];
    }

    template <typename X> requires (!std::is_const_v<Element>)
    auto &operator=(const Element &value)
    {
        *_value = value;
        return *this;
    }

    template <typename X> requires (!std::is_const_v<Element>)
    auto &operator=(Element &&value)
    {
        *_value = std::move(value);
        return *this;
    }

    Element *getValue()
    {
        return _value;
    }

    const Element *getValue() const
    {
        return _value;
    }
};

template <size_t Size>
class ThingSetValue<char[Size]>
    : public ThingSetEncodable, public ThingSetDecodable
{
protected:
    char _value[Size];

public:
    ThingSetValue(const char value[Size])
    {
        memcpy(_value, value, Size * sizeof(char));
    }

    bool encode(ThingSetEncoder &encoder) const override
    {
        return encoder.encode(_value);
    }

    bool decode(ThingSetDecoder &) override
    {
        return false;
    }

    char &operator[](int index)
    {
        return index < Size ? _value[index] : nullptr;
    }

    char *getValue()
    {
        return _value;
    }

    const char *getValue() const
    {
        return _value;
    }
};

}; // namespace ThingSet
