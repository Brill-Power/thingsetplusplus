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

template <NodeBase Base, unsigned Id, unsigned ParentId, StringLiteral Name>
class _IdentifiableThingSetNode : public Base
{
protected:
    _IdentifiableThingSetNode() : Base()
    {}

public:
    constexpr const std::string_view getName() const override
    {
        return Name.string_view();
    }

    constexpr unsigned getId() const override
    {
        return Id;
    }

    constexpr unsigned getParentId() const override
    {
        return ParentId;
    }

    constexpr static const unsigned id = Id;
    constexpr static const std::string_view &name = Name.string_view();
};

/// @brief Represents a ThingSet node with an ID.
/// @tparam Id The unique integer ID of the ThingSet node.
/// @tparam ParentId The integer ID of the parent node.
/// @tparam Name The name of the node.
template <unsigned Id, unsigned ParentId, StringLiteral Name>
class IdentifiableThingSetNode : public _IdentifiableThingSetNode<ThingSetNode, Id, ParentId, Name>
{
protected:
    constexpr IdentifiableThingSetNode() : _IdentifiableThingSetNode<ThingSetNode, Id, ParentId, Name>()
    {
        ThingSetRegistry::registerNode(this);
    }

    ~IdentifiableThingSetNode()
    {
        ThingSetRegistry::unregisterNode(this);
    }
};

/// @brief Represents a ThingSet node with an ID and which may have one or more child nodes.
/// @tparam Id The unique integer ID of the ThingSet node.
/// @tparam ParentId The integer ID of the parent node.
/// @tparam Name The name of the node.
template <unsigned Id, unsigned ParentId, StringLiteral Name>
class IdentifiableThingSetParentNode : public _IdentifiableThingSetNode<ThingSetParentNode, Id, ParentId, Name>
{
protected:
    constexpr IdentifiableThingSetParentNode() : _IdentifiableThingSetNode<ThingSetParentNode, Id, ParentId, Name>()
    {
        ThingSetRegistry::registerNode(this);
    }

    ~IdentifiableThingSetParentNode()
    {
        ThingSetRegistry::unregisterNode(this);
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

} // namespace ThingSet
