/*
 * Copyright (c) 2024 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */
#pragma once

#include "FlatteningIterator.hpp"
#include "ThingSetNode.hpp"
#include <array>
#include <list>

#define NODE_MAP_LOOKUP_BUCKETS 8

namespace ThingSet {

typedef std::list<ThingSetNode *> NodeList;
typedef std::array<NodeList, NODE_MAP_LOOKUP_BUCKETS> NodeMap;

class ThingSetRegistry
{
private:
    static constexpr ThingSetNode _rootNode = ThingSetNode("Root");

    NodeMap _nodeMap;
    ThingSetRegistry();

public:
    // FlatteningIterator<NodeMap::iterator> begin();
    // FlatteningIterator<NodeMap::iterator> end();

    ThingSetRegistry(ThingSetRegistry const &) = delete;
    void operator=(ThingSetRegistry const &) = delete;

    static ThingSetRegistry &getInstance();

    static void registerNode(ThingSetNode *node);

    static bool findById(unsigned id, ThingSetNode **node);

    FlatteningIterator<NodeMap::iterator> begin();
    FlatteningIterator<NodeMap::iterator> end();

    // static void iterateNodes(std::function<void(ThingSetNode *)> action);
};

} // namespace ThingSet