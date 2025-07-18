/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "thingset++/ThingSetAccess.hpp"
#include <string_view>
#include <cstdint>

namespace ThingSet {

enum ThingSetNodeType
{
    hasChildren = 1,
    encodable = 2,
    decodable = 4,
    requestHandler = 8,
    value = 16 | encodable | decodable,
    parameter = 32 | encodable | decodable,
    function = 64 | hasChildren,
    group = 128 | hasChildren,
    record = 256 | hasChildren | requestHandler,
};

struct IntrusiveLinkedListNode
{
    IntrusiveLinkedListNode *next;
};

template <typename T, IntrusiveLinkedListNode T::*Member>
class IntrusiveLinkedList
{
private:
    IntrusiveLinkedListNode *_head;
    IntrusiveLinkedListNode *_tail;

public:
    template <typename Element>
    class IntrusiveLinkedListIterator
    {
    private:
        const IntrusiveLinkedList *_list;
        IntrusiveLinkedListNode *_node;

    public:
        using value_type = Element*;
        using reference = const value_type&;
        using difference_type = ptrdiff_t;

        IntrusiveLinkedListIterator() : _list(nullptr), _node(nullptr)
        {}
        IntrusiveLinkedListIterator(const IntrusiveLinkedList *list, IntrusiveLinkedListNode *node) : _list(list), _node(node)
        {}

        bool operator==(const IntrusiveLinkedListIterator &rhs) const
        {
            return _list == rhs._list && _node == rhs._node;
        }

        bool operator!=(const IntrusiveLinkedListIterator &rhs) const
        {
            return !(*this == rhs);
        }

        IntrusiveLinkedListIterator &operator++()
        {
            _node = _node->next;
            return *this;
        }

        Element *operator*() const
        {
            // https://github.com/boostorg/intrusive/blob/master/include/boost/intrusive/detail/parent_from_member.hpp
            Element *const container = 0;
            const char *const member = static_cast<const char*>(static_cast<const void*>(&(container->*Member)));
            std::ptrdiff_t offset = member - static_cast<const char*>(static_cast<const void*>(container));
            return std::launder(reinterpret_cast<Element *>(reinterpret_cast<std::size_t>(_node) - static_cast<std::size_t>(offset)));
        }
    };

    using iterator = IntrusiveLinkedListIterator<T>;
    using const_iterator = IntrusiveLinkedListIterator<const T>;
    using value_type = T*;
    using reference_type = T*&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;

    IntrusiveLinkedList() : _head(nullptr), _tail(nullptr)
    {}

    IntrusiveLinkedListIterator<T> begin()
    {
        return IntrusiveLinkedListIterator<T>(this, _head);
    }

    IntrusiveLinkedListIterator<T> end()
    {
        return IntrusiveLinkedListIterator<T>(this, nullptr);
    }

    IntrusiveLinkedListIterator<const T> cbegin() const
    {
        return IntrusiveLinkedListIterator<const T>(this, _head);
    }

    IntrusiveLinkedListIterator<const T> cend() const
    {
        return IntrusiveLinkedListIterator<const T>(this, nullptr);
    }

    IntrusiveLinkedListIterator<const T> begin() const
    {
        return cbegin();
    }

    IntrusiveLinkedListIterator<const T> end() const
    {
        return cend();
    }

    void push_back(T *object)
    {
        IntrusiveLinkedListNode *node = &(object->*Member);
        node->next = nullptr; // should we check this first?
        if (_tail == nullptr) {
            _tail = node;
            _head = node;
        } else {
            _tail->next = node;
            _tail = node;
        }
    }

    bool remove (T *object)
    {
        IntrusiveLinkedListNode *node = &(object->*Member);

        IntrusiveLinkedListNode *prev = nullptr;
        IntrusiveLinkedListNode *test;
        for (test = _head; test != nullptr; test = test->next) {
            if (test == node) {
                // remove
                if (prev == nullptr) {
                    _head = node->next;
                    if (_tail == node) {
                        _tail = _head;
                    }
                } else {
                    prev->next = node->next;
                    if (_tail == node) {
                        _tail = prev;
                    }
                }
                node->next = nullptr;
                return true;
            }
            prev = test;
        }
        return false;
    }
};

class ThingSetRegistry;

/// @brief Base class for all nodes.
class ThingSetNode
{
friend class ThingSetRegistry;

private:
    IntrusiveLinkedListNode list;

public:
    constexpr ThingSetNode()
    {}

    constexpr virtual const std::string_view getName() const = 0;

    constexpr virtual uint16_t getId() const = 0;

    constexpr virtual uint16_t getParentId() const = 0;

    constexpr virtual const std::string getType() const = 0;

    virtual bool tryCastTo(ThingSetNodeType type, void **target);

    constexpr virtual ThingSetAccess getAccess() const = 0;

    constexpr virtual uint32_t getSubsets() const
    {
        return 0;
    }
};

} // namespace ThingSet
