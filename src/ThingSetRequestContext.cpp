/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "thingset++/ThingSetRequestContext.hpp"

namespace ThingSet {

ThingSetRequestContext::ThingSetRequestContext(uint8_t *resp, size_t responseLen)
    : index(SIZE_MAX), response(resp), useIds(false)
{}

ThingSetBinaryRequestContext::ThingSetBinaryRequestContext(uint8_t *request, size_t requestLen, uint8_t *resp, size_t responseLen) :
  _ThingSetRequestContext(request, requestLen, resp, responseLen),
  _encoder(resp + 1, responseLen - 1),
  _decoder(request + 1, requestLen - 1, 2)
{}

ThingSetTextRequestContext::ThingSetTextRequestContext(uint8_t *request, size_t requestLen, uint8_t *resp, size_t responseLen) :
  _ThingSetRequestContext(request, requestLen, resp, responseLen),
  _encoder(reinterpret_cast<char *>(resp) + 1, responseLen - 1),
  _decoder(reinterpret_cast<char *>(request) + 1, requestLen - 1)
{}

} // namespace ThingSet