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

template <NodeBase Base>
class _IdentifiableThingSetNode : public Base
{
protected:
    const unsigned _id;
    const unsigned _parentId;
    const std::string_view _name;

    _IdentifiableThingSetNode(const unsigned id, const unsigned parentId, const std::string_view name) : Base(), _id(id), _parentId(parentId), _name(name)
    {}

public:
    const std::string_view getName() const override
    {
        return _name;
    }

    unsigned getId() const override
    {
        return _id;
    }

    unsigned getParentId() const override
    {
        return _parentId;
    }
};

/// @brief Represents a ThingSet node with an ID.
class IdentifiableThingSetNode : public _IdentifiableThingSetNode<ThingSetNode>
{
protected:
    IdentifiableThingSetNode(const unsigned id, const unsigned parentId, const std::string_view name) : _IdentifiableThingSetNode<ThingSetNode>(id, parentId, name)
    {
        ThingSetRegistry::registerNode(this);
    }

    ~IdentifiableThingSetNode()
    {
        ThingSetRegistry::unregisterNode(this);
    }
};

/// @brief Represents a ThingSet node with an ID and which may have one or more child nodes.
class IdentifiableThingSetParentNode : public _IdentifiableThingSetNode<ThingSetParentNode>
{
protected:
    IdentifiableThingSetParentNode(const unsigned id, const unsigned parentId, const std::string_view name) : _IdentifiableThingSetNode<ThingSetParentNode>(id, parentId, name)
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
