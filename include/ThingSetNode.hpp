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
    hasChildren = 1,
    value = 2,
    parameter = 4,
    function = 5,
    group = 7,
    record = 9,
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

    virtual void* castTo(ThingSetNodeType type);
};

} // namespace ThingSet
