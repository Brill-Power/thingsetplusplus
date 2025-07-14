/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "gtest/gtest.h"
#include <thingset++/ThingSet.hpp>

using namespace ThingSet;

TEST(Subsets, Collection)
{
    ThingSetProperty<float> f32 { 0x100, 0, "f32", ThingSetAccess::anyRead, Subset::live };
    ThingSetProperty<int32_t> i32 { 0x200, 0, "i32", ThingSetAccess::anyRead };
    ThingSetProperty<uint32_t> u32 { 0x201, 0, "u32", ThingSetAccess::anyRead, Subset::live };

    size_t count = 0;
    std::array<const char *, 2> names = {
        "f32",
        "u32"
    };
    for (auto node : ThingSetRegistry::nodesInSubset(Subset::live))
    {
        ASSERT_EQ(names[count++], node->getName());
    }

    ASSERT_EQ(2, count);
}

TEST(Subsets, Multiple)
{
    ThingSetProperty<float> f32 { 0x100, 0, "f32", ThingSetAccess::anyReadWrite, Subset::live | Subset::persisted };

    size_t count = 0;
    for (auto node : ThingSetRegistry::nodesInSubset(Subset::live))
    {
        count++;
    }

    ASSERT_EQ(1, count);

    count = 0;
    for (auto node: ThingSetRegistry::nodesInSubset(Subset::persisted))
    {
        count++;
    }

    ASSERT_EQ(1, count);
}