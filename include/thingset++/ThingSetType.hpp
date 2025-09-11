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
    static constexpr StringLiteral name = "record";
};

/// @brief Type of an array.
/// @tparam T The type of the elements in the array.
/// @tparam size The number of elements in the array.
template <typename T, size_t size> struct ThingSetType<std::array<T, size>>
{
    static constexpr StringLiteral name = ThingSetType<T>::name + "[]";
};

/// @brief Type of a function which takes no arguments.
/// @tparam Result The return type of the function.
template <typename Result> struct ThingSetType<std::function<Result()>>
{
    static constexpr StringLiteral name =
        "()->(" + ThingSetType<Result>::name + ")";
};

/// @brief Type of a function which takes one or more arguments.
/// @tparam Result The return type of the function.
/// @tparam ...Args The types of the function arguments.
template <typename Result, typename... Args> struct ThingSetType<std::function<Result(Args...)>>
{
private:
    /// @brief Uses the , operator overload defined on StringLiteral to join
    /// successive types together, each separated by a comma.
    struct _concatenator
    {
        static constexpr StringLiteral value = (ThingSetType<Args>::name, ...);
    };

public:
    static constexpr StringLiteral name =
        "(" + _concatenator::value + ")->(" + ThingSetType<Result>::name + ")";
};

template <typename T> struct ThingSetType<T *>
{
    static constexpr StringLiteral name = ThingSetType<T>::name + "[]";
};

template <typename T, size_t Size> struct ThingSetType<T[Size]>
{
    static constexpr StringLiteral name = ThingSetType<T>::name + "[]";
};

template <size_t Size> struct ThingSetType<char[Size]>
{
    static constexpr StringLiteral name = "string";
};

template <> struct ThingSetType<void>
{
    static constexpr StringLiteral name = "";
};

template <> struct ThingSetType<char>
{
    static constexpr StringLiteral name = "string";
};

template <> struct ThingSetType<const char>
{
    static constexpr StringLiteral name = "string";
};

template <> struct ThingSetType<const char *>
{
    static constexpr StringLiteral name = "string";
};

template <> struct ThingSetType<char *>
{
    static constexpr StringLiteral name = "string";
};

template <> struct ThingSetType<std::string>
{
    static constexpr StringLiteral name = "string";
};

template <> struct ThingSetType<bool>
{
    static constexpr StringLiteral name = "bool";
};

template <> struct ThingSetType<uint8_t>
{
    static constexpr StringLiteral name = "u8";
};

template <> struct ThingSetType<uint16_t>
{
    static constexpr StringLiteral name = "u16";
};

template <> struct ThingSetType<uint32_t>
{
    static constexpr StringLiteral name = "u32";
};

template <> struct ThingSetType<uint64_t>
{
    static constexpr StringLiteral name = "u64";
};

template <> struct ThingSetType<int8_t>
{
    static constexpr StringLiteral name = "i8";
};

template <> struct ThingSetType<int16_t>
{
    static constexpr StringLiteral name = "i16";
};

template <> struct ThingSetType<int32_t>
{
    static constexpr StringLiteral name = "i32";
};

template <> struct ThingSetType<int64_t>
{
    static constexpr StringLiteral name = "i64";
};

template <> struct ThingSetType<float>
{
    static constexpr StringLiteral name = "f32";
};

template <> struct ThingSetType<double>
{
    static constexpr StringLiteral name = "f64";
};

} // namespace ThingSet