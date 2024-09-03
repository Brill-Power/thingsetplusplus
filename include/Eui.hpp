/*
 * Copyright (c) 2024 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */
#pragma once

#include <array>
#include <cstdint>

namespace ThingSet {

class Eui
{
private:
    std::array<uint8_t, 8> value;

    Eui();
    static Eui &getInstance();

public:
    static std::array<uint8_t, 8> getValue();
};

} // namespace ThingSet