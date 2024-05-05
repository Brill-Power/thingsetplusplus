/*
 * Copyright (c) 2024 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */
#include "ThingSetValue.hpp"

using namespace ThingSet;

template <typename T> ThingSetValue<T>::ThingSetValue(T &value) : _value(value)
{}

template <typename T> bool ThingSetValue<T>::encode(ThingSetBinaryEncoder &encoder)
{
    return encoder.encode(_value);
}

template <typename T> T ThingSetValue<T>::operator()
{
    return _value;
}
