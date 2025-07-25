/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "thingset++/ThingSetAccess.hpp"
#include "thingset++/internal/IntrusiveLinkedList.hpp"
#include <string_view>
#include <cstdint>

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

class ThingSetRegistry;
class ThingSetParentNode;

/// @brief Base class for all nodes.
class ThingSetNode
{
friend class ThingSetRegistry;
friend class ThingSetParentNode;

private:
    IntrusiveLinkedListNode list;
    IntrusiveLinkedListNode children;

public:
    constexpr ThingSetNode()
    {}

    constexpr virtual const std::string_view getName() const = 0;

    constexpr virtual uint16_t getId() const = 0;

    constexpr virtual uint16_t getParentId() const = 0;

    constexpr virtual const std::string getType() const = 0;

    virtual bool tryCastTo(ThingSetNodeType type, void **target);

    constexpr virtual ThingSetAccess getAccess() const = 0;

    constexpr virtual uint32_t getSubsets() const
    {
        return 0;
    }
};

} // namespace ThingSet
