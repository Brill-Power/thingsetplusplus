/*
 * Copyright (c) 2024 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */

#include "ThingSetServer.hpp"
#include "ThingSet.hpp"
#include <ThingSetBinaryDecoder.hpp>
#include <ThingSetRegistry.hpp>
#include <list>

namespace ThingSet {

static inline ThingSetProperty<0x1d, 0, "NodeID", std::string> nodeId = std::string("000ba77e71e50000");

ThingSetServer::ThingSetServer(ThingSetServerTransport &transport) : _transport(transport)
{}

std::pair<uint8_t *, size_t> ThingSetServer::requestCallback(uint8_t *buffer, size_t len)
{
    uint8_t error[1];
    uint8_t response[1024];
    FixedSizeThingSetBinaryEncoder encoder(response + 1, sizeof(buffer) - 1);
    FixedSizeThingSetBinaryDecoder decoder(buffer + 1, len - 1, 2);
    std::string path;
    uint16_t id;
    bool useIds = false;
    ThingSetNode *node;
    if (decoder.decode(&path)) {
        if (!ThingSetRegistry::findByName(path, &node)) {
            error[0] = THINGSET_ERR_NOT_FOUND;
            return std::make_pair(error, 1);
        }
    }
    else if (decoder.decode(&id)) {
        if (!ThingSetRegistry::findById(id, &node)) {
            error[0] = THINGSET_ERR_NOT_FOUND;
            return std::make_pair(error, 1);
        }
        useIds = true;
    }
    else {
        // fail
        error[0] = THINGSET_ERR_BAD_REQUEST;
        return std::make_pair(error, 1);
    }
    switch (buffer[0]) {
        case THINGSET_BIN_GET: {
            response[0] = THINGSET_STATUS_CONTENT;
            encoder.encodeNull();
            if (node->getNodeType() == ThingSetNodeType::Value) {
                ThingSetBinaryEncodable *encodable = (ThingSetBinaryEncodable *)node;
                if (encodable->encode(encoder)) {
                    return std::make_pair(response, encoder.getEncodedLength() + 1);
                }
            }
            error[0] = THINGSET_ERR_UNSUPPORTED_FORMAT;
            return std::make_pair(error, 1);
        }
        case THINGSET_BIN_FETCH: {
            response[0] = THINGSET_STATUS_CONTENT;
            encoder.encodeNull();
            if (decoder.decodeNull()) {
                // expect that this is a group
                if (node->getNodeType() == ThingSetNodeType::Group) {
                    ThingSetParentNode *parent = (ThingSetParentNode *)node;
                    if (useIds) {
                        std::list<unsigned> ids;
                        for (ThingSetNode *child : *parent) {
                            ids.push_back(child->getId());
                        }
                        encoder.encode(ids);
                    }
                    else {
                        std::list<std::string_view> names;
                        for (ThingSetNode *child : *parent) {
                            names.push_back(child->getName());
                        }
                        encoder.encode(names);
                    }
                    return std::make_pair(response, encoder.getEncodedLength() + 1);
                }
                else {
                    error[0] = THINGSET_ERR_BAD_REQUEST;
                    return std::make_pair(error, 1);
                }
            }
            else if (decoder.peekType() == ZCBOR_MAJOR_TYPE_LIST && encoder.encodeListStart()) {
                if (decoder.decodeList([node, &decoder, &encoder](size_t index) {
                        unsigned id;
                        if (!decoder.decode(&id)) {
                            return false;
                        }
                        ThingSetNode *n;
                        return ThingSetRegistry::findById(id, &n) && node == ThingSetRegistry::getMetadataNode()
                               && encoder.encodeMapStart() && encoder.encode("name") && encoder.encode(n->getName())
                               && encoder.encode("type") && encoder.encode(n->getType()) && encoder.encodeMapEnd();
                    })
                    && encoder.encodeListEnd())
                {
                    return std::make_pair(response, encoder.getEncodedLength() + 1);
                }
                else {
                    error[0] = THINGSET_ERR_NOT_FOUND;
                    return std::make_pair(error, 1);
                }
            }
        }
        default:
            break;
    }
    error[0] = THINGSET_ERR_NOT_IMPLEMENTED;
    return std::make_pair(error, 1);
}

bool ThingSetServer::listen()
{
    return _transport.listen([this](auto x, auto y) { return requestCallback(x, y); });
}

} // namespace ThingSet