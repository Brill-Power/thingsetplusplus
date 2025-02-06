/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <array>
#include <cstdint>
#include <string>

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
    static const std::string getString();
};

} // namespace ThingSet