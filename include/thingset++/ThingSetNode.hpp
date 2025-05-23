/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
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
    requestHandler = 8,
    value = 16 | encodable | decodable,
    parameter = 32 | encodable | decodable,
    function = 64 | hasChildren,
    group = 128 | hasChildren,
    record = 256 | hasChildren | requestHandler,
};

/// @brief Base class for all nodes.
class ThingSetNode
{
public:
    constexpr ThingSetNode()
    {}

    constexpr virtual const std::string_view getName() const = 0;

    constexpr virtual unsigned getId() const = 0;

    constexpr virtual unsigned getParentId() const = 0;

    virtual const std::string getType() const = 0;

    constexpr virtual ThingSetNodeType getNodeType() const = 0;

    virtual bool tryCastTo(ThingSetNodeType type, void **target);

    virtual ThingSetAccess getAccess() const = 0;

    virtual bool checkAccess(ThingSetAccess access) const = 0;
};

} // namespace ThingSet
