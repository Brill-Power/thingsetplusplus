/*
 * Copyright (c) 2026 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "gtest/gtest.h"
#include <thingset++/ThingSet.hpp>

// >>> NOTE <<<
// 
// If this test binary links successfully, static init didn't trip the
// spurious dedup assert which is a regression in itself

using namespace ThingSet;

namespace {

// Two distinct record types declaring members with the same raw template
// ids (0x7BA, 0x7BB) under different parent record ids (0x7B0, 0x7B1).
// Record members are namespaced by their parent record
struct RecordA
{
    ThingSetReadOnlyRecordMember<0x7BA, 0x7B0, "alpha", uint64_t> alpha;
    ThingSetReadOnlyRecordMember<0x7BB, 0x7B0, "beta", uint32_t> beta;
};

struct RecordB
{
    ThingSetReadOnlyRecordMember<0x7BA, 0x7B1, "alpha", uint64_t> alpha;
    ThingSetReadOnlyRecordMember<0x7BB, 0x7B1, "beta", uint32_t> beta;
};

ThingSetReadOnlyProperty<std::array<RecordA, 2>> recordsA{ 0x7B0, 0x0, "recordsA" };
ThingSetReadOnlyProperty<std::array<RecordB, 1>> recordsB{ 0x7B1, 0x0, "recordsB" };

} // namespace

TEST(RecordMemberNamespaces, BothParentRecordsRegister)
{
    ThingSetNode *node;
    ASSERT_TRUE(ThingSetRegistry::findById(0x7B0, &node));
    EXPECT_EQ("recordsA", node->getName());
    ASSERT_TRUE(ThingSetRegistry::findById(0x7B1, &node));
    EXPECT_EQ("recordsB", node->getName());
}

TEST(RecordMemberNamespaces, FindByIdDisambiguatesByParent)
{
    // Both record types share raw member id 0x7BA. Their proxies land in
    // the same bucket because the parent id contribution to the bucket
    // hash is a multiple of 8 and cancels. findById(rawId, parentId) must
    // therefore also filter on parent id
    ThingSetNode *alphaA = nullptr;
    ASSERT_TRUE(ThingSetRegistry::findById(0x7BA, 0x7B0, &alphaA));
    EXPECT_EQ(0x7B0, alphaA->getParentId());

    ThingSetNode *alphaB = nullptr;
    ASSERT_TRUE(ThingSetRegistry::findById(0x7BA, 0x7B1, &alphaB));
    EXPECT_EQ(0x7B1, alphaB->getParentId());

    EXPECT_NE(alphaA, alphaB) << "findById returned the same proxy for both parents";

    // And again for the second shared id, to cover the neighbouring bucket
    ThingSetNode *betaA = nullptr;
    ASSERT_TRUE(ThingSetRegistry::findById(0x7BB, 0x7B0, &betaA));
    EXPECT_EQ(0x7B0, betaA->getParentId());

    ThingSetNode *betaB = nullptr;
    ASSERT_TRUE(ThingSetRegistry::findById(0x7BB, 0x7B1, &betaB));
    EXPECT_EQ(0x7B1, betaB->getParentId());

    EXPECT_NE(betaA, betaB);
}
