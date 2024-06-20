/*
 * Copyright (c) 2024 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */
#include "IdentifiableThingSetNode.hpp"

// template <unsigned id>
// ThingSet::IdentifiableThingSetNode<id>::IdentifiableThingSetNode(const char *name) : ThingSet::ThingSetNode(name)
// {}

template <unsigned id> constexpr const unsigned ThingSet::IdentifiableThingSetNode<id>::getId() const
{
    return id;
}
