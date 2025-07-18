/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "thingset++/FlatteningIterator.hpp"
#include "thingset++/StringLiteral.hpp"
#include "thingset++/ThingSetParentNode.hpp"
#include <array>
#include <functional>
#include <list>
#include <ranges>

#define NODE_MAP_LOOKUP_BUCKETS 8

namespace ThingSet {

//typedef std::list<ThingSetNode *> NodeList;


/// @brief Repository of all current ThingSet nodes.
class ThingSetRegistry
{
public:
    typedef IntrusiveLinkedList<ThingSetNode, &ThingSetNode::list> NodeList;
    typedef std::array<NodeList, NODE_MAP_LOOKUP_BUCKETS> NodeMap;

private:
    template <uint16_t Id, uint16_t ParentId, StringLiteral Name>
    class OverlayNode : public ThingSetParentNode
    {
    public:
        constexpr virtual const std::string_view getName() const override
        {
            return Name.string_view();
        }

        constexpr virtual uint16_t getId() const override
        {
            return Id;
        }

        constexpr virtual uint16_t getParentId() const override
        {
            return ParentId;
        }

        const std::string getType() const override
        {
            return "group";
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
    FlatteningIterator<NodeMap::const_iterator, std::iterator_traits<NodeMap::const_iterator>::value_type::const_iterator> cbegin() const;
    FlatteningIterator<NodeMap::const_iterator, std::iterator_traits<NodeMap::const_iterator>::value_type::const_iterator> cend() const;
    FlatteningIterator<NodeMap::const_iterator, std::iterator_traits<NodeMap::const_iterator>::value_type::const_iterator> begin() const;
    FlatteningIterator<NodeMap::const_iterator, std::iterator_traits<NodeMap::const_iterator>::value_type::const_iterator> end() const;

    template <typename SubsetType> requires std::is_enum_v<SubsetType>
    static auto nodesInSubset(SubsetType subset)
    {
        uint32_t s = (uint32_t)subset;
        return instance() | std::views::filter([s](auto n) { return (n->getSubsets() & s) == s; });
    }

    static void registerNode(ThingSetNode *node);
    static void unregisterNode(ThingSetNode *node);

    static ThingSetRegistry &instance();

private:
    void registerOrUnregisterNode(ThingSetNode *node,
                                  std::function<void(NodeList &, ThingSetNode *)> nodeListAction,
                                  std::function<bool(ThingSetParentNode *, ThingSetNode *)> parentNodeAction);
};

} // namespace ThingSet