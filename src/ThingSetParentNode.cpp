/*
 * Copyright (c) 2024 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */

#include "ThingSetParentNode.hpp"

ThingSet::ThingSetParentNode::ChildIterator ThingSet::ThingSetParentNode::begin()
{
    return _children.begin();
}

ThingSet::ThingSetParentNode::ChildIterator ThingSet::ThingSetParentNode::end()
{
    return _children.end();
}

bool ThingSet::ThingSetParentNode::addChild(ThingSetNode *child)
{
    _children.push_back(child);
    return true;
}

bool ThingSet::ThingSetParentNode::removeChild(ThingSetNode *child)
{
    _children.remove(child);
    return true;
}
