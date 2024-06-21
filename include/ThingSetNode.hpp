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
private:
    const std::string_view _name;

public:
    constexpr ThingSetNode(const std::string_view name) : _name(name)
    {}

    constexpr const std::string_view getName() const
    {
        return _name;
    }

    constexpr virtual const unsigned getId() const
    {
        return 0;
    }
};

} // namespace ThingSet
