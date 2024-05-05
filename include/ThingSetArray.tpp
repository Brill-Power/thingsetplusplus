/*
 * Copyright (c) 2024 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */
#include "ThingSetArray.hpp"

template <typename T, int size> bool ThingSetArray::encode(ThingSetBinaryEncoder &encoder)
{
    return encoder.encode(_array.data(), _numElements);
}
