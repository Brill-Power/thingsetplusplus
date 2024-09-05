/*
 * Copyright (c) 2024 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */

#pragma once

#include "ThingSetNode.hpp"
#include <list>

namespace ThingSet {

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
};

} // namespace ThingSet