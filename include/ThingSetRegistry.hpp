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
    class RootNode : public ThingSetNode
    {
        RootNode()
        {}
        constexpr const std::string_view getName() const override
        {
            return "Root";
        }

        constexpr const unsigned getId() const override
        {
            return 0;
        }
    };
    static constexpr RootNode _rootNode();

    NodeMap _nodeMap;
    ThingSetRegistry();

public:
    ThingSetRegistry(ThingSetRegistry const &) = delete;
    void operator=(ThingSetRegistry const &) = delete;

    static ThingSetRegistry &getInstance();

    static void registerNode(ThingSetNode *node);

    static bool findById(unsigned id, ThingSetNode **node);

    FlatteningIterator<NodeMap::iterator> begin();
    FlatteningIterator<NodeMap::iterator> end();
};

} // namespace ThingSet