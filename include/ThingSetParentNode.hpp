/*
 * Copyright (c) 2024 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */

#pragma once

#include "ThingSetNode.hpp"
#include <list>

namespace ThingSet {

enum ThingSetCallbackReason
{
    willRead,
    didRead,
    willWrite,
    didWrite
};

/// @brief Storage and behaviour for nodes that have children.
class ThingSetParentNode
{
private:
    std::list<ThingSetNode *> _children;

public:
    typedef typename std::list<ThingSetNode *>::iterator ChildIterator;

    ChildIterator begin();
    ChildIterator end();

    bool addChild(ThingSetNode *child);
    bool removeChild(ThingSetNode *child);

    virtual bool invokeCallback(ThingSetNode *node, ThingSetCallbackReason reason) const = 0;
};

} // namespace ThingSet