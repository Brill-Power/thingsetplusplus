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

template <uint16_t Id, uint16_t ParentId, StringLiteral Name>
class ThingSetGroup : public IdentifiableThingSetParentNode<Id, ParentId, Name>
{
private:
    std::function<bool(ThingSetNode *, ThingSetCallbackReason)> _callback;

public:
    ThingSetGroup() : ThingSetGroup(defaultCallback){};
    ThingSetGroup(std::function<bool(ThingSetNode *, ThingSetCallbackReason)> callback)
        : IdentifiableThingSetParentNode<Id, ParentId, Name>(), _callback(callback){};

    constexpr const std::string getType() const override
    {
        return "group";
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