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

static bool defaultCallback(ThingSetNode *node, ThingSetCallbackReason reason)
{
    return true;
}

template <unsigned Id, unsigned ParentId, StringLiteral Name>
class ThingSetGroup : public IdentifiableThingSetNode<Id, ParentId, Name>, public ThingSetParentNode
{
private:
    std::function<bool(ThingSetNode *, ThingSetCallbackReason)> _callback;

public:
    ThingSetGroup() : ThingSetGroup(defaultCallback){};
    ThingSetGroup(std::function<bool(ThingSetNode *, ThingSetCallbackReason)> callback) : _callback(callback){};

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

    bool checkAccess(ThingSetAccess access) const override
    {
        // no access control on groups at the moment
        return true;
    }

    bool invokeCallback(ThingSetNode *node, ThingSetCallbackReason reason) const override
    {
        return _callback(node, reason);
    }
};

} // namespace ThingSet