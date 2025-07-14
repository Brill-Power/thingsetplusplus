/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "thingset++/IdentifiableThingSetNode.hpp"
#include "thingset++/ThingSetParentNode.hpp"

namespace ThingSet {

static inline bool defaultCallback(ThingSetNode *, ThingSetCallbackReason)
{
    return true;
}

template <unsigned Id, unsigned ParentId, StringLiteral Name>
class ThingSetGroup : public IdentifiableThingSetParentNode
{
private:
    std::function<bool(ThingSetNode *, ThingSetCallbackReason)> _callback;

public:
    ThingSetGroup() : ThingSetGroup(defaultCallback){};
    ThingSetGroup(std::function<bool(ThingSetNode *, ThingSetCallbackReason)> callback)
        : IdentifiableThingSetParentNode(Id, ParentId, Name.string_view()), _callback(callback){};

    constexpr const std::string getType() const override
    {
        return "group";
    }

    constexpr ThingSetNodeType getNodeType() const override
    {
        return ThingSetNodeType::group;
    }

    bool checkAccess(ThingSetAccess) const override
    {
        // no access control on groups at the moment
        return true;
    }

    constexpr ThingSetAccess getAccess() const override
    {
        return ThingSetAccess::anyRead;
    }

    bool invokeCallback(ThingSetNode *node, ThingSetCallbackReason reason) const override
    {
        return _callback(node, reason);
    }
};

} // namespace ThingSet