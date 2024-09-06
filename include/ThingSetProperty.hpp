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

namespace ThingSet {

template <unsigned id, unsigned parentId, StringLiteral name, typename T>
class ThingSetProperty : public ThingSetValue<T>, public IdentifiableThingSetNode<id, parentId, name>
{
public:
    ThingSetProperty() : ThingSetValue<T>(), IdentifiableThingSetNode<id, parentId, name>()
    {}
    ThingSetProperty(const T &value) : ThingSetValue<T>(value), IdentifiableThingSetNode<id, parentId, name>()
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
        return ThingSetType<std::remove_pointer<T>>::name;
    }

    constexpr const ThingSetNodeType getNodeType() const override
    {
        return ThingSetNodeType::value;
    }

    bool tryCastTo(ThingSetNodeType type, void **target) override
    {
        switch (type) {
            case ThingSetNodeType::value:
                *target = static_cast<ThingSetBinaryEncodable *>(this);
                return true;
            default:
                return false;
        }
    }
};

} // namespace ThingSet
