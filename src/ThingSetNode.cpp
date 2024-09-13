/*
 * Copyright (c) 2024 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */

#include "ThingSetNode.hpp"

namespace ThingSet {

bool ThingSetNode::tryCastTo(__attribute_maybe_unused__ ThingSetNodeType type, __attribute_maybe_unused__ void **target)
{
    return false;
}

} // namespace ThingSet