/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
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
    template <unsigned Id, unsigned ParentId, StringLiteral Name> class OverlayNode : public ThingSetParentNode
    {
    public:
        constexpr virtual const std::string_view getName() const override
        {
            return Name.string_view();
        }

        constexpr virtual unsigned getId() const override
        {
            return Id;
        }

        constexpr virtual unsigned getParentId() const override
        {
            return ParentId;
        }

        const std::string getType() const override
        {
            return "group";
        }

        constexpr ThingSetNodeType getNodeType() const override
        {
            return ThingSetNodeType::group;
        }

        bool tryCastTo(ThingSetNodeType type, void **target) override
        {
            switch (type) {
                case ThingSetNodeType::hasChildren:
                    *target = static_cast<ThingSetParentNode *>(this);
                    return true;
                default:
                    return false;
            }
        }

        ThingSetAccess getAccess() const override
        {
            return ThingSetAccess::userRead;
        }

        bool checkAccess(ThingSetAccess) const override
        {
            return true;
        }

        bool invokeCallback(ThingSetNode *, ThingSetCallbackReason) const override
        {
            return true;
        }
    };

    NodeMap _nodeMap;
    OverlayNode<0, 0, ""> _rootNode;
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
    /// @param index When the method returns, contains the array index if relevant.
    /// @return True if the node was found, otherwise false.
    static bool findByName(const std::string &name, ThingSetNode **node, size_t *index);
    /// @brief Find a node by its integer ID.
    /// @param id The integer ID of the node that is sought.
    /// @param node When the method returns, contains a pointer to the node if it was found.
    /// @return True if the node was found, otherwise false.
    static bool findById(const unsigned id, ThingSetNode **node);
    static bool findParentById(const unsigned id, ThingSetParentNode **parent);

    FlatteningIterator<NodeMap::iterator> begin();
    FlatteningIterator<NodeMap::iterator> end();

private:
    static ThingSetRegistry &getInstance();

    static void registerOrUnregisterNode(ThingSetNode *node,
                                         std::function<void(NodeList &, ThingSetNode *)> nodeListAction,
                                         std::function<bool(ThingSetParentNode *, ThingSetNode *)> parentNodeAction);
};

} // namespace ThingSet