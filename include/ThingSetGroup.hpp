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

template <unsigned Id, unsigned ParentId, StringLiteral Name>
class ThingSetGroup : public IdentifiableThingSetNode<Id, ParentId, Name>, public ThingSetParentNode
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

    bool checkAccess(ThingSetAccess access) override
    {
        // no access control on groups at the moment
        return true;
    }
};

} // namespace ThingSet