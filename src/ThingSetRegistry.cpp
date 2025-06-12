/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "thingset++/ThingSetRegistry.hpp"
#include "thingset++/ThingSetParentNode.hpp"
#include "thingset++/internal/logging.hpp"

namespace ThingSet {

ThingSetRegistry::ThingSetRegistry()
{
    // manually register the root node
    _nodeMap[_rootNode.getId() % NODE_MAP_LOOKUP_BUCKETS].push_back(&_rootNode);
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
    if (node->getParentId() != id && findParentById(node->getParentId(), &parent)) {
        parentNodeAction(parent, node);
    }
    void *target;
    if (node->tryCastTo(ThingSetNodeType::hasChildren, &target)) {
        parent = reinterpret_cast<ThingSetParentNode *>(target);
        for (auto n : getInstance()) {
            if (n->getParentId() == node->getId()) {
                parent->addChild(n);
            }
        }
    }
}

void ThingSetRegistry::registerNode(ThingSetNode *node)
{
    LOG_DEBUG("Registering node %s (0x%x)", node->getName().data(), node->getId());
    ThingSetRegistry::registerOrUnregisterNode(
        node, [](auto &l, auto *n) { l.push_back(n); }, [](auto *p, auto *n) { return p->addChild(n); });
}

void ThingSetRegistry::unregisterNode(ThingSetNode *node)
{
    LOG_DEBUG("Unregistering node %s (0x%x)", node->getName().data(), node->getId());
    ThingSetRegistry::registerOrUnregisterNode(
        node, [](auto &l, auto *n) { l.remove(n); }, [](auto *p, auto *n) { return p->removeChild(n); });
}

bool ThingSetRegistry::findParentById(const unsigned id, ThingSetParentNode **parent)
{
    ThingSetNode *node;
    void *target;
    if (findById(id, &node) && node->tryCastTo(ThingSetNodeType::hasChildren, &target)) {
        *parent = reinterpret_cast<ThingSetParentNode *>(target);
        return true;
    }

    return false;
}

bool ThingSetRegistry::findByName(const std::string &name, ThingSetNode **node, size_t *index)
{
    return getInstance()._rootNode.findByName(name, node, index);
}

bool ThingSetRegistry::findById(const unsigned id, ThingSetNode **node)
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