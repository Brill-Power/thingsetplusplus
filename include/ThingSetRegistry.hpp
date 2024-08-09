/*
 * Copyright (c) 2024 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */
#pragma once

#include "FlatteningIterator.hpp"
#include "StringLiteral.hpp"
#include "ThingSetParentNode.hpp"
#include <array>
#include <functional>
#include <list>

#define NODE_MAP_LOOKUP_BUCKETS 8

namespace ThingSet {

typedef std::list<ThingSetNode *> NodeList;
typedef std::array<NodeList, NODE_MAP_LOOKUP_BUCKETS> NodeMap;

/// @brief Repository of all current ThingSet nodes.
class ThingSetRegistry
{
private:
    template <unsigned id, unsigned parentId, StringLiteral name> class OverlayNode : public ThingSetNode
    {
    public:
        constexpr virtual const std::string_view getName() const override
        {
            return name.string_view();
        }

        constexpr virtual const unsigned getId() const override
        {
            return id;
        }

        constexpr virtual const unsigned getParentId() const override
        {
            return parentId;
        }

        const std::string getType() const override
        {
            return "group";
        }

        constexpr const ThingSetNodeType getNodeType() const override
        {
            return ThingSetNodeType::Group;
        }
    };

    class RootNode : public ThingSetNode, public ThingSetParentNode
    {
    private:
        RootNode()
        {}

    public:
        static RootNode &getInstance()
        {
            static RootNode instance;
            return instance;
        }

        constexpr const std::string_view getName() const override
        {
            return "";
        }

        constexpr const unsigned getId() const override
        {
            return 0;
        }

        constexpr const unsigned getParentId() const override
        {
            return 0;
        }

        const std::string getType() const override
        {
            return "group";
        }

        constexpr const ThingSetNodeType getNodeType() const override
        {
            return ThingSetNodeType::Group;
        }
    };

    NodeMap _nodeMap;
    OverlayNode<25, 0, "_Metadata"> _metadataNode;

    ThingSetRegistry();

public:
    ThingSetRegistry(ThingSetRegistry const &) = delete;
    void operator=(ThingSetRegistry const &) = delete;

    static ThingSetNode *getMetadataNode();

    static void registerNode(ThingSetNode *node);
    static void unregisterNode(ThingSetNode *node);

    /// @brief Find a node by its fully-qualified name.
    /// @param name The full string path of the node that is sought.
    /// @param node When the method returns, contains a pointer to the node if it was found.
    /// @return True if the node was found, otherwise false.
    static bool findByName(const std::string &name, ThingSetNode **node);
    /// @brief Find a node by its integer ID.
    /// @param id The integer ID of the node that is sought.
    /// @param node When the method returns, contains a pointer to the node if it was found.
    /// @return True if the node was found, otherwise false.
    static bool findById(unsigned id, ThingSetNode **node);
    static bool findContainerById(unsigned id, ThingSetParentNode **parent);

    FlatteningIterator<NodeMap::iterator> begin();
    FlatteningIterator<NodeMap::iterator> end();

private:
    static ThingSetRegistry &getInstance();

    static void registerOrUnregisterNode(ThingSetNode *node,
                                         std::function<void(NodeList &, ThingSetNode *)> nodeListAction,
                                         std::function<bool(ThingSetParentNode *, ThingSetNode *)> parentNodeAction);
};

} // namespace ThingSet