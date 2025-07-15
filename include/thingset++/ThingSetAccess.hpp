/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <type_traits>
#include <cstdint>

namespace ThingSet {

/// @brief Specifies ThingSet access controls.
enum struct ThingSetAccess : uint8_t
{
    userRead = 1 << 0,
    expertRead = 1 << 1,
    manufacturerRead = 1 << 2,
    anyRead = userRead | expertRead | manufacturerRead,
    userWrite = userRead << 4,
    expertWrite = expertRead << 4,
    manufacturerWrite = manufacturerRead << 4,
    anyWrite = userWrite | expertWrite | manufacturerWrite,
    userReadWrite = userRead | userWrite,
    expertReadWrite = expertRead | expertWrite,
    manufacturerReadWrite = manufacturerRead | manufacturerWrite,
    anyReadWrite = anyRead | anyWrite,
};

constexpr inline ThingSetAccess operator&(const ThingSetAccess &lhs, const ThingSetAccess &rhs)
{
    using T = std::underlying_type_t<ThingSetAccess>;
    return static_cast<ThingSetAccess>(static_cast<T>(lhs) & static_cast<T>(rhs));
}

constexpr inline ThingSetAccess operator|(const ThingSetAccess &lhs, const ThingSetAccess &rhs)
{
    using T = std::underlying_type_t<ThingSetAccess>;
    return static_cast<ThingSetAccess>(static_cast<T>(lhs) | static_cast<T>(rhs));
}

constexpr inline ThingSetAccess &operator|=(ThingSetAccess &lhs, const ThingSetAccess &rhs)
{
    lhs = lhs | rhs;
    return lhs;
}

} // namespace ThingSet