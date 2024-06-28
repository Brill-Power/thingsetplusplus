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
/// @tparam id The unique integer ID of the ThingSet node.
template <unsigned id, StringLiteral name> class IdentifiableThingSetNode : public ThingSetNode
{
public:
    constexpr IdentifiableThingSetNode() : ThingSetNode()
    {
        ThingSetRegistry::getInstance().registerNode(this);
    }

    constexpr const std::string_view getName() const override
    {
        return name.string_view();
    }

    constexpr const unsigned getId() const override
    {
        return id;
    }
};
} // namespace ThingSet
