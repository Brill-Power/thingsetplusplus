/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <cstddef>
#include <new>

namespace ThingSet {

class IntrusiveLinkedListNode;

template <typename T, IntrusiveLinkedListNode T::*Member>
class IntrusiveLinkedList;

/// @brief Holds a pointer to the next member of an intrusive
/// linked list.
class IntrusiveLinkedListNode
{
    template <typename T, IntrusiveLinkedListNode T::*Member>
    friend class IntrusiveLinkedList;

    IntrusiveLinkedListNode *next;

public:
    constexpr IntrusiveLinkedListNode() : next(nullptr)
    {}
};

/// @brief Intrusive linked list.
/// @tparam T The type of elements in the list.
/// @tparam Member The member in the type which contains the list
/// member pointer.
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

    size_t count() const
    {
        size_t count = 0;
        for ([[maybe_unused]] const T *_ : *this)
        {
            count++;
        }
        return count;
    }

    void push_back(T &object)
    {
        push_back(&object);
    }

    void push_back(T *object)
    {
        IntrusiveLinkedListNode *node = &(object->*Member);
        if (node->next) {
            // what is the right behaviour here?
            // we could just set it to null and carry on, but...
            return;
        }
        if (_tail == nullptr) {
            _tail = node;
            _head = node;
        } else {
            _tail->next = node;
            _tail = node;
        }
    }

    bool remove(T &object)
    {
        return remove(&object);
    }

    bool remove(T *object)
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

} // namespace ThingSet