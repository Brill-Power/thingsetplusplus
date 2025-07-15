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
    ThingSetReadOnlyProperty<float, Subset::live> f32 { 0x100, 0, "f32" };
    ThingSetReadOnlyProperty<int32_t> i32 { 0x200, 0, "i32" };
    ThingSetReadOnlyProperty<uint32_t, Subset::live> u32 { 0x201, 0, "u32" };

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
    ThingSetReadWriteProperty<float, Subset::live | Subset::persisted> f32 { 0x100, 0, "f32" };

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