/*
 * Copyright (c) 2024 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */
#pragma once

#include "ThingSetNode.hpp"
#include "ThingSetRegistry.hpp"

namespace ThingSet {

/// @brief Represents a ThingSet node with an ID.
/// @tparam id The unique integer ID of the ThingSet node.
template <unsigned id> class IdentifiableThingSetNode : public ThingSetNode
{
public:
    constexpr IdentifiableThingSetNode(const std::string_view name) : ThingSetNode(name)
    {
        ThingSetRegistry::getInstance().registerNode(this);
    }

    constexpr const unsigned getId() const override;
};
} // namespace ThingSet

#include "IdentifiableThingSetNode.tpp"