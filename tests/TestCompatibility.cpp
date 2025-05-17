/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "thingset++/ThingSet.hpp"
#include "thingset++/ThingSetCompatibility.hpp"
#include "gtest/gtest.h"

using namespace ThingSet;

TEST(Compatibility, SimpleProperty)
{
    float f;
    THINGSET_ADD_ITEM_FLOAT(0x0, 0x100, "sFloat", &f, 0, THINGSET_ANY_R | THINGSET_MFR_W, 0)

    char s[] = "Hello";
    THINGSET_ADD_ITEM_STRING(0x0, 0x101, "sString", s, strlen(s), THINGSET_ANY_R | THINGSET_MFR_W, 0)
}