/*
 * Copyright (c) 2024 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */
#pragma once

#include "IdentifiableThingSetNode.hpp"
#include "StringLiteral.hpp"
#include "ThingSetValue.hpp"

namespace ThingSet {
template <unsigned id, unsigned parentId, StringLiteral name, typename T>
class ThingSetParameter : public ThingSetValue<T>, public IdentifiableThingSetNode<id, parentId, name>
{
public:
    ThingSetParameter() : ThingSetValue<T>()
    {}

    const std::string getType() const override
    {
        return ThingSetType<T>::name;
    }

    constexpr const ThingSetNodeType getNodeType() const override
    {
        return ThingSetNodeType::parameter;
    }
};

} // namespace ThingSet
