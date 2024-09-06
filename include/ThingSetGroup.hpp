/*
 * Copyright (c) 2024 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */

#pragma once

#include "IdentifiableThingSetNode.hpp"
#include "StringLiteral.hpp"
#include "ThingSetParentNode.hpp"

namespace ThingSet {

template <unsigned id, unsigned parentId, StringLiteral name>
class ThingSetGroup : public IdentifiableThingSetNode<id, parentId, name>, public ThingSetParentNode
{
public:
    const std::string getType() const override
    {
        return "group";
    }

    constexpr const ThingSetNodeType getNodeType() const override
    {
        return ThingSetNodeType::group;
    }

    void* castTo(ThingSetNodeType type) override {
        switch (type) {
            case ThingSetNodeType::hasChildren:
                return static_cast<ThingSetParentNode *>(this);
            default:
                return 0;
        }
    }
};

} // namespace ThingSet