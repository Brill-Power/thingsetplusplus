/*
 * Copyright (c) 2024 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */
#pragma once

#include "IdentifiableThingSetNode.hpp"
#include "StringLiteral.hpp"
#include "ThingSetType.hpp"
#include <array>

namespace ThingSet {

template <unsigned id, unsigned parentId, StringLiteral name, typename Element, typename Collection>
class ThingSetRecordCollection : public ThingSetValue<Collection>, public IdentifiableThingSetNode<id, parentId, name>
{
public:
    const std::string getType() const override
    {
        return ThingSetType<Element>::name;
    }

    constexpr const ThingSetNodeType getNodeType() const override
    {
        return ThingSetNodeType::Record;
    }
};

template <unsigned id, unsigned parentId, StringLiteral name, typename Element, unsigned size>
class ThingSetRecordArray : public ThingSetRecordCollection<id, parentId, name, Element, std::array<Element, size>>
{
public:
    ThingSetRecordArray()
    {}

    ThingSetRecordArray(std::array<Element, size> &source) : ThingSetValue<std::array<Element, size>>(source)
    {}

    template <typename... Elements> ThingSetRecordArray(Elements... args)
    {
        initialiseArray(args...);
    }

    Element &operator[](int index)
    {
        return this->_value[index];
    }

private:
    void initialiseArray()
    {}
    template <typename... Elements> void initialiseArray(Element arg, Elements... remainder)
    {
        const size_t index = size - (sizeof...(Elements) + 1);
        this->_value[index] = arg;
        initialiseArray(remainder...);
    }
};

template <unsigned id, unsigned parentId, StringLiteral name, typename Element>
class ThingSetRecordList : public ThingSetRecordCollection<id, parentId, name, Element, std::list<Element>>
{
};

} // namespace ThingSet