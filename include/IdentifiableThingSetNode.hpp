/*
 * Copyright (c) 2024 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */
#pragma once

#include "StringLiteral.hpp"
#include "ThingSetNode.hpp"
#include "ThingSetRegistry.hpp"

namespace ThingSet {

/// @brief Represents a ThingSet node with an ID.
/// @tparam id_ The unique integer ID of the ThingSet node.
/// @tparam parentId_ The integer ID of the parent node.
/// @tparam name The name of the node.
template <unsigned id_, unsigned parentId_, StringLiteral name> class IdentifiableThingSetNode : public ThingSetNode
{
public:
    constexpr IdentifiableThingSetNode() : ThingSetNode()
    {
        ThingSetRegistry::getInstance().registerNode(this);
    }

    ~IdentifiableThingSetNode()
    {
        ThingSetRegistry::getInstance().unregisterNode(this);
    }

    constexpr const std::string_view getName() const override
    {
        return name.string_view();
    }

    constexpr const unsigned getId() const override
    {
        return id_;
    }

    constexpr const unsigned getParentId() const override
    {
        return parentId_;
    }

    constexpr static const unsigned groupId = parentId_;

    constexpr static const unsigned id = id_;
};
} // namespace ThingSet
