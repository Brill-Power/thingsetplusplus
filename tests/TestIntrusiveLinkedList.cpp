/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "gtest/gtest.h"
#include "thingset++/internal/IntrusiveLinkedList.hpp"

using namespace ThingSet;

struct Something
{
    IntrusiveLinkedListNode node;
    int index;
};

TEST(Intrusion, AddRemove)
{
    IntrusiveLinkedList<Something, &Something::node> list;
    Something s;
    s.index = 1;
    list.push_back(s);
    ASSERT_EQ(1, list.count());
    ASSERT_TRUE(list.remove(s));
    ASSERT_EQ(0, list.count());
}

TEST(Intrusion, AddRemoveNonExistent)
{
    IntrusiveLinkedList<Something, &Something::node> list;
    Something s;
    s.index = 1;
    list.push_back(s);
    ASSERT_EQ(1, list.count());
    Something t;
    t.index = 2;
    ASSERT_FALSE(list.remove(t));
    ASSERT_EQ(1, list.count());
}

TEST(Intrusion, AddRemoveInBetween)
{
    IntrusiveLinkedList<Something, &Something::node> list;
    Something s;
    s.index = 1;
    list.push_back(s);
    ASSERT_EQ(1, list.count());
    Something t;
    t.index = 2;
    list.push_back(t);
    Something u;
    t.index = 3;
    list.push_back(u);
    Something v;
    t.index = 4;
    list.push_back(v);
    ASSERT_EQ(4, list.count());
    list.remove(u);
    ASSERT_EQ(3, list.count());
    list.remove(t);
    ASSERT_EQ(2, list.count());
}

TEST(Intrusion, Iterate)
{
    IntrusiveLinkedList<Something, &Something::node> list;
    std::array<Something, 10> somethings;
    for (size_t i = 0; i < somethings.size(); i++)
    {
        Something &something = somethings[i];
        something.index = i;
        list.push_back(something);
    }
    ASSERT_EQ(10, list.count());
    size_t index = 0;
    for (auto *sth : list)
    {
        Something *origin = &somethings[index++];
        ASSERT_EQ(origin, sth);
    }
}

struct SomethingDerived : public Something
{
    int otherIndex;
};

TEST(Intrusion, AddIterateDerived)
{
    IntrusiveLinkedList<Something, &Something::node> list;
    SomethingDerived s;
    s.index = 1;
    s.otherIndex = 1;
    list.push_back(s);
    ASSERT_EQ(1, list.count());
    for (Something *s : list)
    {
        ASSERT_EQ(1, s->index);
    }
}

struct Both
{
    IntrusiveLinkedListNode first;
    IntrusiveLinkedListNode second;
    size_t index;
};

TEST(Intrusion, MoreThanOneList)
{
    IntrusiveLinkedList<Both, &Both::first> first;
    IntrusiveLinkedList<Both, &Both::second> second;
    Both b;
    b.index = 1;
    first.push_back(b);
    ASSERT_EQ(1, first.count());
    ASSERT_EQ(0, second.count());
    second.push_back(b);
    ASSERT_EQ(1, first.count());
    ASSERT_EQ(1, second.count());
    first.remove(b);
    ASSERT_EQ(0, first.count());
    ASSERT_EQ(1, second.count());
}