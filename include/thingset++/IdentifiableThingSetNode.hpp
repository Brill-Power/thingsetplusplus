/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "thingset++/StringLiteral.hpp"
#include "thingset++/ThingSetNode.hpp"
#include "thingset++/ThingSetRegistry.hpp"

namespace ThingSet {

template <typename T>
concept NodeBase = std::is_base_of_v<ThingSetNode, T>;

/// @brief Represents a ThingSet node with an ID.
template <uint16_t Id, uint16_t ParentId, StringLiteral Name, NodeBase Base>
class _IdentifiableThingSetNode : public Base
{
protected:
    _IdentifiableThingSetNode() : ThingSetNode()
    {
        ThingSetRegistry::registerNode(this);
    }

    ~_IdentifiableThingSetNode()
    {
        ThingSetRegistry::unregisterNode(this);
    }

public:
    const std::string_view getName() const override
    {
        return Name.string_view();
    }

    uint16_t getId() const override
    {
        return Id;
    }

    uint16_t getParentId() const override
    {
        return ParentId;
    }
};

/// @brief Represents a ThingSet node with an ID and which may have one or more child nodes.
template <uint16_t Id, uint16_t ParentId, StringLiteral Name>
class _IdentifiableThingSetNode<Id, ParentId, Name, ThingSetParentNode> : public ThingSetParentNode
{
protected:
    _IdentifiableThingSetNode() : ThingSetParentNode()
    {
        ThingSetRegistry::registerNode(this);
    }

    ~_IdentifiableThingSetNode()
    {
        ThingSetRegistry::unregisterNode(this);
    }

public:
    const std::string_view getName() const override
    {
        return Name.string_view();
    }

    uint16_t getId() const override
    {
        return Id;
    }

    uint16_t getParentId() const override
    {
        return ParentId;
    }

    bool tryCastTo(ThingSetNodeType type, void **target) override
    {
        switch (type) {
            case ThingSetNodeType::hasChildren:
                *target = static_cast<ThingSetParentNode *>(this);
                return true;
            default:
                return false;
        }
    }
};

template <uint16_t Id, uint16_t ParentId, StringLiteral Name>
using IdentifiableThingSetNode = _IdentifiableThingSetNode<Id, ParentId, Name, ThingSetNode>;
template <uint16_t Id, uint16_t ParentId, StringLiteral Name>
using IdentifiableThingSetParentNode = _IdentifiableThingSetNode<Id, ParentId, Name, ThingSetParentNode>;

} // namespace ThingSet
