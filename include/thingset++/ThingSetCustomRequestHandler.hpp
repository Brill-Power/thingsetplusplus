/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "thingset++/ThingSetRequestContext.hpp"

namespace ThingSet {

/// @brief Facilitates custom handling of requests.
class ThingSetCustomRequestHandler
{
public:
    /// @brief Provide custom handling of a request.
    /// @param context The request context.
    /// @return The length of the response if the request was handled, or 0 if it was not.
    virtual int handleRequest(ThingSetRequestContext &context) = 0;
};

} // namespace ThingSet