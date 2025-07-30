/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <cstdint>
#include <type_traits>

namespace ThingSet {

template <typename T>
concept StreamingMessageType = requires
{
    T::first;
    T::consecutive;
    T::single;
    T::last;
} and std::is_enum_v<T>;

} // ThingSet