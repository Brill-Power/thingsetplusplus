/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <type_traits>

namespace ThingSet {

enum ThingSetAccess
{
    userRead = 1,
    userWrite = 2,
    advancedRead = 8,
    advancedWrite = 16,
    manufacturerRead = 64,
    manufacturerWrite = 128,
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