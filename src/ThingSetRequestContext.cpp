/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "thingset++/ThingSetRequestContext.hpp"

namespace ThingSet {

ThingSetRequestContext::ThingSetRequestContext(uint8_t *request, size_t requestLength, uint8_t *response)
    : _request(request), _requestLength(requestLength), _response(response), index(SIZE_MAX)
{}

uint16_t &ThingSetRequestContext::id()
{
    return _id.value();
}

std::string &ThingSetRequestContext::path()
{
    return _path.value();
}

bool ThingSetRequestContext::hasValidEndpoint()
{
    return _id.has_value() || _path.has_value();
}

bool ThingSetRequestContext::isToBeForwarded()
{
    return _path.has_value() && _path.value().length() > 1 && _path.value()[0] == '/';
}

bool ThingSetRequestContext::useIds()
{
    return _id.has_value();
}

ThingSetBinaryRequestContext::ThingSetBinaryRequestContext(uint8_t *request, size_t requestLen, uint8_t *resp, size_t responseLen) :
    _ThingSetRequestContext(request, requestLen, resp),
    _encoder(resp + 1, responseLen - 1),
    _decoder(request + 1, requestLen - 1, 2)
{
    std::string path;
    uint16_t id;
    if (_decoder.decode(&path))
    {
        _path = path;
    }
    else if (_decoder.decode(&id))
    {
        _id = id;
    }
}

bool ThingSetBinaryRequestContext::setStatus(const ThingSetStatusCode &status)
{
    _response[0] = status;
    return true;
}

ThingSetTextRequestContext::ThingSetTextRequestContext(uint8_t *request, size_t requestLen, uint8_t *resp, size_t responseLen) :
    _ThingSetRequestContext(request, requestLen, resp),
    _encoder(reinterpret_cast<char *>(resp) + 4, responseLen - 4),
    _decoder(reinterpret_cast<char *>(request) + 1, requestLen - 1)
{
    // find first space, if any
    char *pathStart = reinterpret_cast<char *>(request) + 1;
    char *pathEnd = (char *)memchr(pathStart, ' ', requestLen);
    if (pathEnd != nullptr)
    {
        _path = std::string(pathStart, pathEnd - pathStart);
        _decoder = DefaultFixedSizeThingSetTextDecoder(reinterpret_cast<char *>(pathEnd + 1), requestLen - 1 - (pathEnd - pathStart));
    }
    else
    {
        _path = std::string(pathStart, requestLen - 1);
    }
}

bool ThingSetTextRequestContext::setStatus(const ThingSetStatusCode &status)
{
    _response[0] = ':';
    snprintf(reinterpret_cast<char *>(&_response[1]), 4, "%.2X", status);
    _response[3] = ' ';
    return true;
}

} // namespace ThingSet