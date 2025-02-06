/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "thingset++/ThingSetRequestContext.hpp"

namespace ThingSet {

ThingSetRequestContext::ThingSetRequestContext(uint8_t *request, size_t requestLen, uint8_t *resp, size_t responseLen)
    : index(SIZE_MAX), requestType((ThingSetRequestType)request[0]), response(resp), useIds(false),
      encoder(resp + 1, responseLen - 1), decoder(request + 1, requestLen - 1, 2)
{}

} // namespace ThingSet