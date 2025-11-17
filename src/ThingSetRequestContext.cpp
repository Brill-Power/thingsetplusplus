/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "thingset++/ThingSetRequestContext.hpp"
#include "thingset++/internal/logging.hpp"

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
    return isForward() || (_path.has_value() && _path.value().length() > 1 && _path.value()[0] == '/');
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

    nodeId = path().substr(1, pos - 1);
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

size_t ThingSetBinaryRequestContext::rewrite([[maybe_unused]] uint8_t **request, [[maybe_unused]] size_t requestLength, [[maybe_unused]] std::string &nodeId)
{
#ifdef ENABLE_GATEWAY
    if (isForward())
    {
        // new style; CBOR encoded node ID as string
        uint8_t *req = *request;
        if (ZCBOR_MAJOR_TYPE(req[1]) == ZCBOR_MAJOR_TYPE_TSTR) {
            uint8_t length = ZCBOR_ADDITIONAL(req[1]);
            // assume less than 24 chars
            if (length <= ZCBOR_VALUE_IN_HEADER) {
                nodeId = std::string(&req[2], &req[2] + length);
                *request = &(*request)[1 + 1 + length]; // request byte plus CBOR header
                return requestLength - (1 + 1 + length);
            } else {
                LOG_WARN("Invalid request; node ID too long.");
                return 0;
            }
        }
        return 0;
    }
    else
    {
        // old-style node ID in path
        if (!tryGetNodeId(nodeId)) {
            return 0;
        }

        // first find string in buffer
        uint8_t *requestEnd = &(*request)[requestLength - 1];
        uint8_t *nodeIdPathComponentStart = std::search(*request, requestEnd, nodeId.begin(), nodeId.end());
        if (nodeIdPathComponentStart == requestEnd)
        {
            return 0;
        }

        // calculate new start of path as an offset
        size_t newPathStart = (nodeIdPathComponentStart + nodeId.length() + 1) - *request;
        // new length is old length less node ID and two slashes
        size_t newPathLength = path().length() - nodeId.length() - 2;
        // write new string header
        // depending on string length need 1 or 2 bytes to
        if (newPathLength <= ZCBOR_VALUE_IN_HEADER) {
            (*request)[--newPathStart] = 0x60 | newPathLength;
        } else {
            (*request)[--newPathStart] = newPathLength;
            (*request)[--newPathStart] = 0x78; // i.e. 0x60 | 0x18 (single byte integer follows)
        }
        // finally, copy verb from first byte
        (*request)[--newPathStart] = (*request)[0];
        size_t newLength = requestLength - newPathStart + 1;
        *request = &(*request)[newPathStart];
        return newLength;
    }
#else
    return 0;
#endif // ENABLE_GATEWAY
}

bool ThingSetBinaryRequestContext::setStatus(const ThingSetStatusCode &status)
{
    _response[0] = status;
    return true;
}

#ifdef ENABLE_TEXT_MODE
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

size_t ThingSetTextRequestContext::rewrite([[maybe_unused]] uint8_t **request, [[maybe_unused]] size_t requestLength, [[maybe_unused]] std::string &nodeId)
{
#ifdef ENABLE_GATEWAY
    if (isForward())
    {
        uint8_t *req = *request;
        size_t i = 1;
        for (; i < requestLength; i++)
        {
            uint8_t c = req[i];
            if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
            {
                continue;
            }
            else
            {
                // have found first non-hex char
                break;
            }
        }

        nodeId = std::string(&req[1], &req[i]);
        *request = &(*request)[i];
        return requestLength - i;
    }
    else
    {
        if (!tryGetNodeId(nodeId))
        {
            return 0;
        }

        // assuming ?/deadbeef12345678/Foo, we can take the first byte and
        // replace the slash after the node ID
        request[nodeId.length() + 2] = request[0];
        size_t newLength = requestLength - (nodeId.length() + 2);
        *request = &(*request)[nodeId.length() + 2];
        return newLength;
    }
#else
    return 0;
#endif // ENABLE_GATEWAY
}

bool ThingSetTextRequestContext::setStatus(const ThingSetStatusCode &status)
{
    _response[0] = ':';
    snprintf(reinterpret_cast<char *>(&_response[1]), 4, "%.2X", status);
    _response[3] = ' ';
    return true;
}
#endif // ENABLE_TEXT_MODE

} // namespace ThingSet