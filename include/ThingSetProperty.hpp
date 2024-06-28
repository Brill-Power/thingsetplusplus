/*
 * Copyright (c) 2024 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */
#pragma once

#include "IdentifiableThingSetNode.hpp"
#include "StringLiteral.hpp"
#include "ThingSetValue.hpp"
#include <array>

namespace ThingSet {

template <unsigned id, StringLiteral name, typename T>
class ThingSetProperty : public ThingSetValue<T>, public IdentifiableThingSetNode<id, name>
{
public:
    ThingSetProperty() : IdentifiableThingSetNode<id, name>(), ThingSetValue<T>()
    {}
    ThingSetProperty(T value) : IdentifiableThingSetNode<id, name>(), ThingSetValue<T>(value)
    {}
};

} // namespace ThingSet
