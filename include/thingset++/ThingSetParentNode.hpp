/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "thingset++/ThingSetNode.hpp"
#include "thingset++/IntrusiveLinkedList.hpp"
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

/// @brief Storage and behaviour for nodes that have children.
class ThingSetParentNode : public ThingSetNode
{
private:
    IntrusiveLinkedList<ThingSetNode, &ThingSetNode::children> _children;

public:
    typedef IntrusiveLinkedList<ThingSetNode, &ThingSetNode::children>::iterator ChildIterator;

public:
    virtual ChildIterator begin();
    ChildIterator end();

    bool addChild(ThingSetNode *child);
    bool removeChild(ThingSetNode *child);

    bool tryCastTo(ThingSetNodeType type, void **target) override;

    virtual bool findByName(const std::string &name, ThingSetNode **node, size_t *index);

    virtual bool invokeCallback(ThingSetNode *node, ThingSetCallbackReason reason) const = 0;
};

} // namespace ThingSet