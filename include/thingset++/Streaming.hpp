/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <cstdint>

namespace ThingSet {

template <typename T>
concept StreamingMessageType = requires
{
    T::first;
    T::consecutive;
    T::single;
    T::last;
};

} // ThingSet