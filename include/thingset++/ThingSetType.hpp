/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <cstdint>
#include <string>

namespace ThingSet {

template <typename T> struct ThingSetType
{
    inline static const std::string name = "record";
};

template <typename T, size_t size> struct ThingSetType<std::array<T, size>>
{
    inline static const std::string name = std::string(ThingSetType<T>::name) + "[]";
};

static inline std::string concatAndShift(std::string &str)
{
    return str;
}

template <typename Arg, typename... Remainder> static std::string concatAndShift(std::string &str)
{
    str += (str.length() > 0 ? "," : "") + ThingSetType<Arg>::name;
    if constexpr (sizeof...(Remainder) > 0) {
        return concatAndShift<Remainder...>(str);
    }
    else {
        return concatAndShift(str);
    }
}

template <typename... Args> static std::string concat()
{
    std::string result;
    if constexpr (sizeof...(Args) > 0) {
        concatAndShift<Args...>(result);
    }
    return result;
}

template <typename Result, typename... Args> struct ThingSetType<std::function<Result(Args...)>>
{
    inline static const std::string name =
        "(" + concat<Args...>() + ")->(" + std::string(ThingSetType<Result>::name) + ")";
};

template <> struct ThingSetType<void>
{
    inline static const std::string name = "";
};

template <> struct ThingSetType<const char *>
{
    inline static const std::string name = "string";
};

template <> struct ThingSetType<char *>
{
    inline static const std::string name = "string";
};

template <> struct ThingSetType<std::string>
{
    inline static const std::string name = "string";
};

template <> struct ThingSetType<bool>
{
    inline static const std::string name = "bool";
};

template <> struct ThingSetType<uint8_t>
{
    inline static const std::string name = "u8";
};

template <> struct ThingSetType<uint16_t>
{
    inline static const std::string name = "u16";
};

template <> struct ThingSetType<uint32_t>
{
    inline static const std::string name = "u32";
};

template <> struct ThingSetType<uint64_t>
{
    inline static const std::string name = "u64";
};

template <> struct ThingSetType<int8_t>
{
    inline static const std::string name = "i8";
};

template <> struct ThingSetType<int16_t>
{
    inline static const std::string name = "i16";
};

template <> struct ThingSetType<int32_t>
{
    inline static const std::string name = "i32";
};

template <> struct ThingSetType<int64_t>
{
    inline static const std::string name = "i64";
};

template <> struct ThingSetType<float>
{
    inline static const std::string name = "f32";
};

template <> struct ThingSetType<double>
{
    inline static const std::string name = "f64";
};

} // namespace ThingSet