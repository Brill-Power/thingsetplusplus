/*
 * Copyright (c) 2024 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */
#pragma once

#include <string_view>

namespace ThingSet {

class ThingSetNode
{
public:
    constexpr ThingSetNode()
    {}

    constexpr virtual const std::string_view getName() const
    {
        return nullptr;
    }

    constexpr virtual const unsigned getId() const
    {
        return 0;
    }
};

} // namespace ThingSet
