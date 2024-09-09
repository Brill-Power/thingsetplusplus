/*
 * Copyright (c) 2024 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */
#pragma once

#include "IdentifiableThingSetNode.hpp"
#include "StringLiteral.hpp"
#include "ThingSetAccess.hpp"
#include "ThingSetType.hpp"
#include "ThingSetValue.hpp"

namespace ThingSet {

template <unsigned Id, unsigned ParentId, StringLiteral Name, ThingSetAccess Access, typename T>
class _ThingSetProperty : public ThingSetValue<T>, public IdentifiableThingSetNode<Id, ParentId, Name>
{
protected:
    _ThingSetProperty() : ThingSetValue<T>(), IdentifiableThingSetNode<Id, ParentId, Name>()
    {}
    _ThingSetProperty(const T &value) : ThingSetValue<T>(value), IdentifiableThingSetNode<Id, ParentId, Name>()
    {}

    const std::string getType() const override
    {
        return ThingSetType<std::remove_pointer_t<T>>::name;
    }

    constexpr const ThingSetNodeType getNodeType() const override
    {
        return ThingSetNodeType::value;
    }

    bool tryCastTo(ThingSetNodeType type, void **target) override
    {
        switch (type) {
            case ThingSetNodeType::encodable:
                *target = static_cast<ThingSetBinaryEncodable *>(this);
                return true;
            case ThingSetNodeType::decodable:
                *target = static_cast<ThingSetBinaryDecodable *>(this);
                return true;
            default:
                return false;
        }
    }

    bool checkAccess(ThingSetAccess access) const override
    {
        return (Access & access) == Access;
    }
};

template <unsigned Id, unsigned ParentId, StringLiteral Name, ThingSetAccess Access, typename T>
class ThingSetProperty : public _ThingSetProperty<Id, ParentId, Name, Access, T>
{
public:
    ThingSetProperty() : _ThingSetProperty<Id, ParentId, Name, Access, T>::_ThingSetProperty()
    {}
    ThingSetProperty(const T &value) : _ThingSetProperty<Id, ParentId, Name, Access, T>::_ThingSetProperty(value)
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
};

template <unsigned Id, unsigned ParentId, StringLiteral Name, ThingSetAccess Access, typename Element, std::size_t Size>
class ThingSetProperty<Id, ParentId, Name, Access, std::array<Element, Size>>
    : public _ThingSetProperty<Id, ParentId, Name, Access, std::array<Element, Size>>, ThingSetParentNode
{
public:
    ThingSetProperty() : _ThingSetProperty<Id, ParentId, Name, Access, std::array<Element, Size>>::_ThingSetProperty()
    {}
    ThingSetProperty(const std::array<Element, Size> &value)
        : _ThingSetProperty<Id, ParentId, Name, Access, std::array<Element, Size>>::_ThingSetProperty(value)
    {}

    auto &operator=(const std::array<Element, Size> &value)
    {
        this->_value = value;
        return *this;
    }

    auto &operator=(std::array<Element, Size> &&value)
    {
        this->_value = std::move(value);
        return *this;
    }

    bool tryCastTo(ThingSetNodeType type, void **target) override
    {
        if (_ThingSetProperty<Id, ParentId, Name, Access, std::array<Element, Size>>::tryCastTo(type, target)) {
            return true;
        }

        if (type == ThingSetNodeType::hasChildren) {
            *target = static_cast<ThingSetParentNode *>(this);
            return true;
        }

        return false;
    }
};

template <unsigned Id, unsigned ParentId, StringLiteral Name, typename T>
using ThingSetReadOnlyProperty = ThingSetProperty<Id, ParentId, Name, ThingSetAccess::userRead, T>;

template <unsigned Id, unsigned ParentId, StringLiteral Name, typename T>
using ThingSetReadWriteProperty =
    ThingSetProperty<Id, ParentId, Name, ThingSetAccess::userRead | ThingSetAccess::userWrite, T>;

template <unsigned Id, unsigned ParentId, StringLiteral Name, typename T>
using ThingSetReadAdvancedWriteProperty =
    ThingSetProperty<Id, ParentId, Name, ThingSetAccess::userRead | ThingSetAccess::advancedWrite, T>;

template <unsigned Id, unsigned ParentId, StringLiteral Name, typename T>
using ThingSetReadManufacturerWriteProperty =
    ThingSetProperty<Id, ParentId, Name, ThingSetAccess::userRead | ThingSetAccess::manufacturerWrite, T>;

} // namespace ThingSet
