/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "thingset++/ThingSetRequestContext.hpp"

namespace ThingSet {

ThingSetRequestContext::ThingSetRequestContext(uint8_t *resp)
    : _response(resp), index(SIZE_MAX), useIds(false)
{}

ThingSetBinaryRequestContext::ThingSetBinaryRequestContext(uint8_t *request, size_t requestLen, uint8_t *resp, size_t responseLen) :
    _ThingSetRequestContext(request, resp),
    _encoder(resp + 1, responseLen - 1),
    _decoder(request + 1, requestLen - 1, 2)
{}

bool ThingSetBinaryRequestContext::setStatus(const ThingSetStatusCode &status)
{
    _response[0] = status;
    return true;
}

ThingSetTextRequestContext::ThingSetTextRequestContext(uint8_t *request, size_t requestLen, uint8_t *resp, size_t responseLen) :
    _ThingSetRequestContext(request, resp),
    _encoder(reinterpret_cast<char *>(resp) + 4, responseLen - 4),
    _decoder(reinterpret_cast<char *>(request) + 1, requestLen - 1)
{}

bool ThingSetTextRequestContext::setStatus(const ThingSetStatusCode &status)
{
    _response[0] = ':';
    snprintf(reinterpret_cast<char *>(&_response[1]), 4, "%.2X", status);
    _response[3] = ' ';
    return true;
}

} // namespace ThingSet