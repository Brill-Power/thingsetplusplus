/*
 * Copyright (c) 2024 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */

#include "ThingSetParentNode.hpp"

namespace ThingSet {

ThingSetParentNode::ChildIterator ThingSetParentNode::begin()
{
    return _children.begin();
}

ThingSetParentNode::ChildIterator ThingSetParentNode::end()
{
    return _children.end();
}

bool ThingSetParentNode::addChild(ThingSetNode *child)
{
    _children.push_back(child);
    return true;
}

bool ThingSetParentNode::removeChild(ThingSetNode *child)
{
    _children.remove(child);
    return true;
}

} // namespace ThingSet