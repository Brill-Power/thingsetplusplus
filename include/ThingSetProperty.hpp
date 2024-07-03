/*
 * Copyright (c) 2024 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */
#pragma once

#include "IdentifiableThingSetNode.hpp"
#include "StringLiteral.hpp"
#include "ThingSetType.hpp"
#include "ThingSetValue.hpp"
#include <array>

namespace ThingSet {

template <unsigned id, unsigned parentId, StringLiteral name, typename T>
class ThingSetProperty : public ThingSetValue<T>, public IdentifiableThingSetNode<id, parentId, name>
{
public:
    ThingSetProperty() : IdentifiableThingSetNode<id, parentId, name>(), ThingSetValue<T>()
    {}
    ThingSetProperty(const T &value) : IdentifiableThingSetNode<id, parentId, name>(), ThingSetValue<T>(value)
    {}

    auto &operator=(const T &value)
    {
        this->_value = value;
        return *this;
    }

    auto &operator=(T &&value)
    {
        this->_value = std::move(value);
        return *this;
    }

    const std::string getType() const override
    {
        return ThingSetType<T>::name;
    }

    constexpr const ThingSetNodeType getNodeType() const override
    {
        return ThingSetNodeType::Value;
    }
};

} // namespace ThingSet
