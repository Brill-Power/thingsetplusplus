/*
 * Copyright (c) 2024 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */
#pragma once

#include <string_view>

namespace ThingSet {

enum ThingSetNodeType
{
    Value,
    Group,
};

class ThingSetNode
{
public:
    constexpr ThingSetNode()
    {}

    constexpr virtual const std::string_view getName() const = 0;

    constexpr virtual const unsigned getId() const = 0;

    constexpr virtual const unsigned getParentId() const = 0;

    virtual const std::string getType() const = 0;

    constexpr virtual const ThingSetNodeType getNodeType() const = 0;
};

} // namespace ThingSet
