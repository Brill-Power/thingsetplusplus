/*
 * Copyright (c) 2024 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */
#pragma once

#include <array>

template <typename T, int size> class ThingSetArray : public ThingSetBinaryEncodable
{
private:
    std::array<T, size> _array;
    size_t _numElements;

public:
    bool encode(ThingSetBinaryEncoder &encoder) override;
};

#include "ThingSetArray.tpp"
