/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "thingset++/IdentifiableThingSetNode.hpp"
#include "thingset++/ThingSetEncoder.hpp"
#include "thingset++/ThingSetParentNode.hpp"

namespace ThingSet {

static inline bool defaultCallback(ThingSetNode *, ThingSetCallbackReason)
{
    return true;
}

template <uint16_t Id, uint16_t ParentId, StringLiteral Name>
class ThingSetGroup : public IdentifiableThingSetParentNode<Id, ParentId, Name>, public ThingSetEncodable
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

    bool tryCastTo(ThingSetNodeType type, void **target) override
    {
        if (type == ThingSetNodeType::encodable) {
            *target = static_cast<ThingSetEncodable *>(this);
            return true;
        }
        if (type == ThingSetNodeType::group) {
            // Marker cast so callers can distinguish groups from other
            // hasChildren nodes (e.g. records)
            return true;
        }
        return IdentifiableThingSetParentNode<Id, ParentId, Name>::tryCastTo(type, target);
    }

    /// @brief Emit the group's encodable children as a nested map. This lets
    /// groups appear as values in their parent's rendering (e.g. `?` at root
    /// now returns `{"GroupA":{...},"GroupB":{...},...}` instead of skipping
    /// groups entirely). Nesting recurses naturally because encodable children
    /// that are themselves groups dispatch to this same override
    ///
    /// If the encoder requests skeleton rendering (see
    /// ThingSetEncoder::renderGroupAsSkeleton), only child groups are emitted;
    /// leaf values (properties, records, arrays) are suppressed
    bool encode(ThingSetEncoder &encoder) const override
    {
        // const_cast: ThingSetParentNode::begin/end are non-const, but we only
        // read the child list here
        auto *self = const_cast<ThingSetGroup *>(this);
        const bool skeleton = encoder.renderGroupAsSkeleton();
        void *target;

        auto shouldInclude = [&](ThingSetNode *child) {
            if (!child->tryCastTo(ThingSetNodeType::encodable, &target)) {
                return false;
            }
            if (skeleton && !child->tryCastTo(ThingSetNodeType::group, &target)) {
                return false;
            }
            return true;
        };

        size_t count = 0;
        for (ThingSetNode *child : *self) {
            if (shouldInclude(child)) {
                count++;
            }
        }
        if (!encoder.encodeMapStart(count)) {
            return false;
        }
        for (ThingSetNode *child : *self) {
            if (!shouldInclude(child)) {
                continue;
            }
            child->tryCastTo(ThingSetNodeType::encodable, &target);
            ThingSetEncodable *encodable = reinterpret_cast<ThingSetEncodable *>(target);
            bool ok = encoder.encodeKeysAsIds()
                ? encoder.encode(std::make_pair(child->getId(), encodable))
                : encoder.encode(std::make_pair(child->getName(), encodable));
            if (!ok) {
                return false;
            }
        }
        return encoder.encodeMapEnd(count);
    }
};

} // namespace ThingSet