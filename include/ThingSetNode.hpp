/*
 * Copyright (c) 2024 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */
#pragma once

#include "ThingSetAccess.hpp"
#include <string_view>

namespace ThingSet {

enum ThingSetNodeType
{
    hasChildren = 1,
    encodable = 2,
    decodable = 4,
    value = 8 | encodable | decodable,
    parameter = 16 | encodable | decodable,
    function = 32 | hasChildren,
    group = 64 | hasChildren,
    record = 128 | hasChildren,
};

/// @brief Base class for all nodes.
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

    virtual bool tryCastTo(ThingSetNodeType type, void **target);

    virtual bool checkAccess(ThingSetAccess access) const = 0;
};

} // namespace ThingSet
