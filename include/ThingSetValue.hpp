/*
 * Copyright (c) 2024 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */
#pragma once

#include "ThingSetBinaryEncoder.hpp"

template <typename T> class ThingSetValue : public ThingSetBinaryEncodable
{
protected:
    T _value;

public:
    ThingSetValue(T &value);

    bool encode(ThingSetBinaryEncoder &encoder) override;

    operator T();
};

#include "ThingSetValue.tpp"
