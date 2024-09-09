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
    std::array<uint8_t, 8> _array;
    uint64_t _value;

    Eui();
    static Eui &getInstance();

public:
    static const uint64_t &getValue();
    static const std::array<uint8_t, 8> &getArray();
};

} // namespace ThingSet