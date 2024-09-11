/*
 * Copyright (c) 2024 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */

#include "ThingSetRequestContext.hpp"

namespace ThingSet {

ThingSetRequestContext::ThingSetRequestContext(uint8_t *request, size_t requestLen, uint8_t *resp, size_t responseLen)
    : index(SIZE_MAX), requestType((ThingSetRequestType)request[0]), useIds(false), response(resp),
      encoder(resp + 1, responseLen - 1), decoder(request + 1, requestLen - 1, 2)
{}

} // namespace ThingSet