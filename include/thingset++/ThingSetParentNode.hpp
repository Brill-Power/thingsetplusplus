/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "thingset++/ThingSetNode.hpp"
#include "thingset++/internal/IntrusiveLinkedList.hpp"
#include <list>

namespace ThingSet {

/// @brief Specifies the reason a group or record callback is invoked.
enum ThingSetCallbackReason
{
    /// @brief A property value is about to be read.
    willRead,
    /// @brief A property value was read.
    didRead,
    /// @brief A property value is about to be written.
    willWrite,
    /// @brief A property value was written.
    didWrite
};

class ThingSetRegistry;

/// @brief Storage and behaviour for nodes that have children.
class ThingSetParentNode : public ThingSetNode
{
friend class ThingSetRegistry;

private:
    IntrusiveLinkedList<ThingSetNode, &ThingSetNode::children> _children;

public:
    typedef IntrusiveLinkedList<ThingSetNode, &ThingSetNode::children>::iterator ChildIterator;

    virtual ChildIterator begin();
    ChildIterator end();

    bool tryCastTo(ThingSetNodeType type, void **target) override;

    virtual bool findByName(const std::string &name, ThingSetNode **node, size_t *index);

    virtual bool invokeCallback(ThingSetNode *node, ThingSetCallbackReason reason) const = 0;

private:
    bool addChild(ThingSetNode *child);
    bool removeChild(ThingSetNode *child);
};

} // namespace ThingSet