/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "thingset++/ThingSetParentNode.hpp"
#include <cstdint>
#include <string>

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

bool ThingSetParentNode::tryCastTo(ThingSetNodeType type, void **target)
{
    if (type == ThingSetNodeType::hasChildren) {
        *target = static_cast<ThingSetParentNode *>(this);
        return true;
    }

    return false;
}

bool ThingSetParentNode::findByName(const std::string &name, ThingSetNode **node, size_t *index)
{
    *index = SIZE_MAX;

    size_t pos;
    std::string token = name;
    *node = this;
    pos = name.find('/', 0);
    token = name.substr(0, pos);
    if ((*node)->getName() == token && pos == std::string::npos) {
        return true;
    }
    for (ThingSetNode *child : *this) {
        if (child->getName() == token) {
            *node = child;
            if (pos == std::string::npos) {
                return true;
            }
            void *target;
            if ((*node)->tryCastTo(ThingSetNodeType::hasChildren, &target)) {
                auto *parent = reinterpret_cast<ThingSetParentNode *>(target);
                return parent->findByName(name.substr(token.length() + 1), node, index);
            }
            else {
                break;
            }
        }
    }

    return false;
}

} // namespace ThingSet