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

/// @brief Context for a single ThingSet request.
struct ThingSetRequestContext
{
    ThingSetRequestContext(uint8_t *request, size_t requestLen, uint8_t *resp, size_t responseLen);

    /// @brief Pointer to the ThingSet node this request relates to.
    ThingSetNode *node;
    /// @brief If the node is an array, indicates the array element to which the request relates.
    size_t index;
    /// @brief Specifies the request type.
    ThingSetRequestType requestType;
    /// @brief Pointer to the buffer into which the response will be written.
    uint8_t *response;
    /// @brief If true, keys in responses will be encoded as integer IDs; if false, as strings.
    bool useIds;
    /// @brief The path to the node to which this request relates.
    std::string path;
    /// @brief The encoder that will encode any response.
    DefaultFixedDepthThingSetBinaryEncoder encoder;
    /// @brief The decoder that will decode the incoming request.
    DefaultFixedDepthThingSetBinaryDecoder decoder;
};

} // namespace ThingSet