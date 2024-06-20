/*
 * Copyright (c) 2024 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */
#pragma once

#include "IdentifiableThingSetNode.hpp"
#include "ThingSetValue.hpp"

namespace ThingSet {

template <unsigned id, typename T> class ThingSetProperty : public ThingSetValue<T>, public IdentifiableThingSetNode<id>
{
public:
    ThingSetProperty(std::string_view name, T value);
};

} // namespace ThingSet

#include "ThingSetProperty.tpp"