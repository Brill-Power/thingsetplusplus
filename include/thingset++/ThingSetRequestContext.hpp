/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
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
    uint8_t *response;
    bool useIds;
    std::string path;
    DefaultFixedDepthThingSetBinaryEncoder encoder;
    DefaultFixedDepthThingSetBinaryDecoder decoder;
};

} // namespace ThingSet