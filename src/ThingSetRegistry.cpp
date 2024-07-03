/*
 * Copyright (c) 2024 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */

#include "ThingSetRegistry.hpp"
#include <ThingSetParentNode.hpp>
#ifdef DEBUG_LOGGING
#include <iostream>
#endif

namespace ThingSet {

ThingSetRegistry::ThingSetRegistry()
{
    // manually register the root node
    _nodeMap[RootNode::getInstance().getId() % NODE_MAP_LOOKUP_BUCKETS].push_back(&RootNode::getInstance());
    _nodeMap[_metadataNode.getId() % NODE_MAP_LOOKUP_BUCKETS].push_back(&_metadataNode);
}

ThingSetNode *ThingSetRegistry::getMetadataNode()
{
    return &getInstance()._metadataNode;
}

ThingSetRegistry &ThingSetRegistry::getInstance()
{
    static ThingSetRegistry instance;
    return instance;
}

void ThingSetRegistry::registerOrUnregisterNode(
    ThingSetNode *node, std::function<void(NodeList &, ThingSetNode *)> nodeListAction,
    std::function<bool(ThingSetParentNode *, ThingSetNode *)> parentNodeAction)
{
    unsigned id = node->getId();
    NodeList &list = getInstance()._nodeMap[id % NODE_MAP_LOOKUP_BUCKETS];
    nodeListAction(list, node);
    ThingSetParentNode *parent;
    if (node->getParentId() != id && findContainerById(node->getParentId(), &parent)) {
        parentNodeAction(parent, node);
    }
}

void ThingSetRegistry::registerNode(ThingSetNode *node)
{
#ifdef DEBUG_LOGGING
    std::cout << "Registering node " << node->getName() << " (" << node->getId() << ")" << std::endl;
#endif
    ThingSetRegistry::registerOrUnregisterNode(
        node, [](auto &l, auto *n) { l.push_back(n); }, [](auto *p, auto *n) { return p->addChild(n); });
}

void ThingSetRegistry::unregisterNode(ThingSetNode *node)
{
#ifdef DEBUG_LOGGING
    std::cout << "Unregistering node " << node->getName() << " (" << node->getId() << ")" << std::endl;
#endif
    ThingSetRegistry::registerOrUnregisterNode(
        node, [](auto &l, auto *n) { l.remove(n); }, [](auto *p, auto *n) { return p->removeChild(n); });
}

bool ThingSetRegistry::findContainerById(unsigned id, ThingSetParentNode **parent)
{
    ThingSetNode *node;
    if (findById(id, &node) && node->getNodeType() == ThingSetNodeType::Group) {
        *parent = (ThingSetParentNode *)node;
    }

    return false;
}

bool ThingSetRegistry::findByName(const std::string &name, ThingSetNode **node)
{
    size_t pos = 0;
    size_t index;
    std::string token = name;
    *node = &RootNode::getInstance();
    do {
        index = name.find('/', pos);
        token = name.substr(pos, index);
        if ((*node)->getName() == token) {
            break;
        }
        if ((*node)->getNodeType() != ThingSetNodeType::Group) {
            break;
        }
        auto *parent = (ThingSetParentNode *)*node;
        for (ThingSetNode *child : *parent) {
            if (child->getName() == token) {
                pos += index;
                *node = child;
                break;
            }
        }
    } while (index != std::string::npos);
    if ((*node)->getName() == name) {
        return true;
    }

    return false;
}

bool ThingSetRegistry::findById(unsigned id, ThingSetNode **node)
{
    NodeList list = getInstance()._nodeMap[id % NODE_MAP_LOOKUP_BUCKETS];
    for (ThingSetNode *n : list) {
        if (n && (n->getId() == id)) {
            *node = n;
            return true;
        }
    }
    return false;
}

FlatteningIterator<NodeMap::iterator> ThingSetRegistry::begin()
{
    auto begin = _nodeMap.begin();
    auto end = _nodeMap.end();
    return flatten(begin, end);
}

FlatteningIterator<NodeMap::iterator> ThingSetRegistry::end()
{
    auto end = _nodeMap.end();
    return flatten(end);
}

} // namespace ThingSet