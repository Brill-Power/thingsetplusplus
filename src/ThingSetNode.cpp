/*
 * Copyright (c) 2024 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */

#include "ThingSetNode.hpp"

namespace ThingSet {

bool ThingSetNode::tryCastTo([[__unused]] ThingSetNodeType type, [[__unused]] void **target)
{
    return false;
}

} // namespace ThingSet