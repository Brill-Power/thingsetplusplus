/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "gtest/gtest.h"
#include <thingset++/ThingSet.hpp>
#include <thingset++/Eui.hpp>

using namespace ThingSet;

TEST(Eui, AllRepresentationsAreSame)
{
    std::string string = Eui::getString();
    auto arr = Eui::getArray();
    uint64_t t = Eui::getValue();
    char buffer[128];
    snprintf(buffer, sizeof(buffer), "%16llX", t);
    ASSERT_STREQ(string.c_str(), buffer);
    snprintf(buffer, sizeof(buffer), "%02X%02X%02X%02X%02X%02X%02X%02X", arr[0], arr[1], arr[2], arr[3], arr[4], arr[5],
             arr[6], arr[7]);
    ASSERT_STREQ(string.c_str(), buffer);
}