/*
 * Copyright (c) 2024 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */

#include "ThingSetRegistry.hpp"
#include "ThingSetParentNode.hpp"
#ifdef __ZEPHYR__
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(thingsetplusplus, CONFIG_THINGSET_PLUS_PLUS_LOG_LEVEL);
#else
#include "internal/logging.hpp"
#endif

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
    if (node->getParentId() != id && findContainerById(node->getParentId(), &parent)) {
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
    LOG_DBG("Registering node %s (0x%x)", node->getName().data(), node->getId());
    ThingSetRegistry::registerOrUnregisterNode(
        node, [](auto &l, auto *n) { l.push_back(n); }, [](auto *p, auto *n) { return p->addChild(n); });
}

void ThingSetRegistry::unregisterNode(ThingSetNode *node)
{
    LOG_DBG("Unregistering node %s (0x%x)", node->getName().data(), node->getId());
    ThingSetRegistry::registerOrUnregisterNode(
        node, [](auto &l, auto *n) { l.remove(n); }, [](auto *p, auto *n) { return p->removeChild(n); });
}

bool ThingSetRegistry::findContainerById(const unsigned id, ThingSetParentNode **parent)
{
    ThingSetNode *node;
    void *target;
    if (findById(id, &node) && node->tryCastTo(ThingSetNodeType::hasChildren, &target)) {
        *parent = reinterpret_cast<ThingSetParentNode *>(target);
        return true;
    }

    return false;
}

bool ThingSetRegistry::findByName(const std::string &name, ThingSetNode **node)
{
    size_t pos = 0;
    size_t index;
    std::string token = name;
    *node = &getInstance()._rootNode;
    do {
        index = name.find('/', pos);
        token = name.substr(pos, index);
        if ((*node)->getName() == token) {
            break;
        }
        void *target;
        if (!(*node)->tryCastTo(ThingSetNodeType::hasChildren, &target)) {
            break;
        }
        auto *parent = reinterpret_cast<ThingSetParentNode *>(target);
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