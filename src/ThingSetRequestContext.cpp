/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "thingset++/ThingSetRequestContext.hpp"

namespace ThingSet {

ThingSetRequestContext::ThingSetRequestContext(uint8_t *response)
    : _response(response), index(SIZE_MAX)
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

bool ThingSetRequestContext::tryGetNodeId(std::string &nodeId)
{
    // first find nodeID by finding next slash
    const size_t pos = path().find('/', 1);
    if (pos == std::string::npos)
    {
        return false;
    }

    nodeId = path().substr(1, pos);
    return true;
}

ThingSetBinaryRequestContext::ThingSetBinaryRequestContext(uint8_t *request, size_t requestLen, uint8_t *response, size_t responseSize) :
    _ThingSetRequestContext(request, response),
    _encoder(response + 1, responseSize - 1),
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

uint8_t *ThingSetBinaryRequestContext::rewrite(uint8_t *request, size_t requestLength, std::string &nodeId)
{
    if (!tryGetNodeId(nodeId)) {
        return nullptr;
    }

    // first find string in buffer
    uint8_t *requestEnd = &request[requestLength - 1];
    uint8_t *nodeIdPathComponentStart = std::search(request, requestEnd, nodeId.begin(), nodeId.end());
    if (nodeIdPathComponentStart == requestEnd)
    {
        return nullptr;
    }

    // calculate new start of path as an offset
    size_t newPathStart = (nodeIdPathComponentStart + nodeId.length()) - request;
    // new length is old length less node ID and two slashes
    size_t newPathLength = path().length() - nodeId.length() - 2;
    // write new string header
    // depending on string length need 1 or 2 bytes to
    if (newPathLength < 25) {
        request[--newPathStart] = 0x60 | newPathLength;
    } else {
        request[--newPathStart] = newPathLength;
        request[--newPathStart] = 0x78;
    }
    // finally, copy verb from first byte
    request[--newPathStart] = request[0];
    return &request[newPathStart];
}

bool ThingSetBinaryRequestContext::setStatus(const ThingSetStatusCode &status)
{
    _response[0] = status;
    return true;
}

ThingSetTextRequestContext::ThingSetTextRequestContext(uint8_t *request, size_t requestLen, uint8_t *response, size_t responseSize) :
    _ThingSetRequestContext(request, response),
    _encoder(reinterpret_cast<char *>(response) + 4, responseSize - 4),
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

uint8_t *ThingSetTextRequestContext::rewrite(uint8_t *request, size_t requestLength, std::string &nodeId)
{
    if (!tryGetNodeId(nodeId)) {
        return nullptr;
    }

    // assuming ?/deadbeef12345678/Foo, we can take the first byte and
    // replace the slash after the node ID
    request[nodeId.length() + 2] = request[0];
    return &request[nodeId.length() + 2];
}

bool ThingSetTextRequestContext::setStatus(const ThingSetStatusCode &status)
{
    _response[0] = ':';
    snprintf(reinterpret_cast<char *>(&_response[1]), 4, "%.2X", status);
    _response[3] = ' ';
    return true;
}

} // namespace ThingSet