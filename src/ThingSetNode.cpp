/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "thingset++/ThingSetNode.hpp"

namespace ThingSet {

bool ThingSetNode::tryCastTo(ThingSetNodeType, void **)
{
    return false;
}

} // namespace ThingSet