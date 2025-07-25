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
    return &instance()._metadataNode;
}

ThingSetRegistry &ThingSetRegistry::instance()
{
    static ThingSetRegistry instance;
    return instance;
}

void ThingSetRegistry::registerOrUnregisterNode(
    ThingSetNode *node, std::function<void(NodeList &, ThingSetNode *)> nodeListAction,
    std::function<bool(ThingSetParentNode *, ThingSetNode *)> parentNodeAction)
{
    unsigned id = node->getId();
    NodeList &list = _nodeMap[id % NODE_MAP_LOOKUP_BUCKETS];
    nodeListAction(list, node);
    ThingSetParentNode *parent;
    if (node->getParentId() != id && findParentById(node->getParentId(), &parent)) {
        parentNodeAction(parent, node);
    }
}

void ThingSetRegistry::registerNode(ThingSetNode *node)
{
    LOG_DEBUG("Registering node %s (0x%x)", node->getName().data(), node->getId());
    ThingSetRegistry::instance().registerOrUnregisterNode(
        node, [](auto &l, auto *n) { l.push_back(n); }, [](auto *p, auto *n) { return p->addChild(n); });
    void *target;
    if (node->tryCastTo(ThingSetNodeType::hasChildren, &target)) {
        ThingSetParentNode *parent = reinterpret_cast<ThingSetParentNode *>(target);
        for (auto n : instance()) {
            if (n->getParentId() == node->getId()) {
                parent->addChild(n);
            }
        }
    }
}

void ThingSetRegistry::unregisterNode(ThingSetNode *node)
{
    LOG_DEBUG("Unregistering node %s (0x%x)", node->getName().data(), node->getId());
    ThingSetRegistry::instance().registerOrUnregisterNode(
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

bool ThingSetRegistry::findByName(const std::string &name, ThingSetNode **node)
{
    size_t index;
    return instance()._rootNode.findByName(name, node, &index);
}

bool ThingSetRegistry::findByName(const std::string &name, ThingSetNode **node, size_t *index)
{
    return instance()._rootNode.findByName(name, node, index);
}

bool ThingSetRegistry::findById(const unsigned id, ThingSetNode **node)
{
    NodeList list = instance()._nodeMap[id % NODE_MAP_LOOKUP_BUCKETS];
    for (ThingSetNode *n : list) {
        if (n && (n->getId() == id)) {
            *node = n;
            return true;
        }
    }
    return false;
}

FlatteningIterator<ThingSetRegistry::NodeMap::iterator> ThingSetRegistry::begin()
{
    auto begin = instance()._nodeMap.begin();
    auto end = instance()._nodeMap.end();
    return flatten(begin, end);
}

FlatteningIterator<ThingSetRegistry::NodeMap::const_iterator, std::iterator_traits<ThingSetRegistry::NodeMap::const_iterator>::value_type::const_iterator> ThingSetRegistry::cbegin() const
{
    auto begin = instance()._nodeMap.cbegin();
    auto end = instance()._nodeMap.cend();
    return flatten<NodeMap::const_iterator, std::iterator_traits<NodeMap::const_iterator>::value_type::const_iterator>(begin, end);
}

FlatteningIterator<ThingSetRegistry::NodeMap::const_iterator, std::iterator_traits<ThingSetRegistry::NodeMap::const_iterator>::value_type::const_iterator> ThingSetRegistry::begin() const
{
    return cbegin();
}

FlatteningIterator<ThingSetRegistry::NodeMap::iterator> ThingSetRegistry::end()
{
    auto end = instance()._nodeMap.end();
    return flatten(end);
}

FlatteningIterator<ThingSetRegistry::NodeMap::const_iterator, std::iterator_traits<ThingSetRegistry::NodeMap::const_iterator>::value_type::const_iterator> ThingSetRegistry::cend() const
{
    auto end = instance()._nodeMap.cend();
    return flatten<ThingSetRegistry::NodeMap::const_iterator, std::iterator_traits<ThingSetRegistry::NodeMap::const_iterator>::value_type::const_iterator>(end);
}

FlatteningIterator<ThingSetRegistry::NodeMap::const_iterator, std::iterator_traits<ThingSetRegistry::NodeMap::const_iterator>::value_type::const_iterator> ThingSetRegistry::end() const
{
    return cend();
}

} // namespace ThingSet