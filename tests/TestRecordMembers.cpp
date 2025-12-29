/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "gtest/gtest.h"
#include <thingset++/ThingSet.hpp>

using namespace ThingSet;

struct Child
{
    ThingSetReadOnlyRecordMember<0x511, 0x510, "voltage", float> voltage;
};

struct Record
{
    ThingSetReadOnlyRecordMember<0x501, 0x500, "voltage", float> voltage;
    ThingSetReadOnlyRecordMember<0x502, 0x500, "current", float> current;
    ThingSetReadOnlyRecordMember<0x503, 0x500, "error", uint64_t> error;
    ThingSetReadOnlyRecordMember<0x510, 0x500, "cells", std::array<Child, 4>> cells;
};

ThingSetReadOnlyProperty<std::array<Record, 2>> records { 0x500, 0x0, "records" };

TEST(RecordMembers, FindRecordById)
{
    ThingSetNode *node;
    ASSERT_TRUE(ThingSetRegistry::findById(0x500, &node));
    ASSERT_EQ("records", node->getName());
}

TEST(RecordMembers, FindRecordByName)
{
    ThingSetNode *node;
    ASSERT_TRUE(ThingSetRegistry::findByName("records", &node));
    ASSERT_EQ(0x500, node->getId());
}

TEST(RecordMembers, FindValueRecordMemberById)
{
    ThingSetNode *node;
    ASSERT_TRUE(ThingSetRegistry::findById(0x501, 0x500, &node));
    ASSERT_EQ("voltage", node->getName());
}

TEST(RecordMembers, FindArrayRecordMemberById)
{
    ThingSetNode *node;
    ASSERT_TRUE(ThingSetRegistry::findById(0x510, 0x500, &node));
    ASSERT_EQ("cells", node->getName());
}

TEST(RecordMembers, ArrayRecordMemberHasChildren)
{
    ThingSetNode *node;
    ASSERT_TRUE(ThingSetRegistry::findById(0x510, 0x500, &node));
    void *target;
    ASSERT_TRUE(node->tryCastTo(ThingSetNodeType::hasChildren, &target));
    ThingSetParentNode &parent = *reinterpret_cast<ThingSetParentNode *>(target);
    size_t count = 0;
    for (auto n : parent) {
        ASSERT_EQ("voltage", n->getName());
        ASSERT_EQ(0x511, n->getId());
        count++;
    }
    ASSERT_EQ(1, count);
}