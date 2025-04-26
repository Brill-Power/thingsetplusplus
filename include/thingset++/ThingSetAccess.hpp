/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <type_traits>

namespace ThingSet {

/// @brief Specifies ThingSet access controls.
enum ThingSetAccess
{
    userRead = 1 << 0,
    userWrite = 1 << 1,
    advancedRead = 1 << 3,
    advancedWrite = 1 << 4,
    manufacturerRead = 1 << 6,
    manufacturerWrite = 1 << 7,
};

constexpr inline ThingSetAccess operator|(ThingSetAccess lhs, ThingSetAccess rhs)
{
    using T = std::underlying_type_t<ThingSetAccess>;
    return static_cast<ThingSetAccess>(static_cast<T>(lhs) | static_cast<T>(rhs));
}

constexpr inline ThingSetAccess &operator|=(ThingSetAccess &lhs, ThingSetAccess &rhs)
{
    lhs = lhs | rhs;
    return lhs;
}

} // namespace ThingSet