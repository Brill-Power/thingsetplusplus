/*
 * Copyright (c) 2024 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */

#pragma once

#include "ThingSetBinaryDecoder.hpp"
#include "ThingSetBinaryEncoder.hpp"
#include "ThingSetNode.hpp"
#include "ThingSetStatus.hpp"

namespace ThingSet {

struct ThingSetRequestContext
{
    ThingSetRequestContext(uint8_t *request, size_t requestLen, uint8_t *resp, size_t responseLen);

    ThingSetNode *node;
    size_t index;
    ThingSetRequestType requestType;
    DefaultFixedSizeThingSetBinaryDecoder decoder;
    DefaultFixedSizeThingSetBinaryEncoder encoder;
    uint8_t *response;
    bool useIds;
    std::string path;
};

} // namespace ThingSet