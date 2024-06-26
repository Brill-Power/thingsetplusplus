/*
 * Copyright (c) 2024 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */
#pragma once

#include "ThingSetBinaryEncoder.hpp"

namespace ThingSet {

/// @brief Represents a ThingSet value of a given type.
/// @tparam T The type of the value.
template <typename T> class ThingSetValue : public ThingSetBinaryEncodable
{
protected:
    T _value;

public:
    ThingSetValue(T value);

    bool encode(ThingSetBinaryEncoder &encoder) override;

    T &operator()();
    const T &operator()() const;

    auto &operator=(const T &value);
};

}; // namespace ThingSet

#include "ThingSetValue.tpp"
