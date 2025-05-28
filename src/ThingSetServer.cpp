/*
 * Copyright (c) 2024-2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "thingset++/ThingSetServer.hpp"
#include "thingset++/Eui.hpp"
#include "thingset++/ThingSet.hpp"
#include "thingset++/ThingSetCustomRequestHandler.hpp"
#include "thingset++/ThingSetRegistry.hpp"
#include <list>

namespace ThingSet {

static inline ThingSetProperty<0x1d, 0, "NodeID", ThingSetAccess::userRead, std::string> nodeId(Eui::getString());
static const uint16_t MetadataNameId = 0x1a;
static const uint16_t MetadataTypeId = 0x1b;
static const uint16_t MetadataAccessId = 0x1c;

_ThingSetServer::_ThingSetServer() : _access(ThingSetAccess::anyReadWrite)
{}

int _ThingSetServer::handleGet(ThingSetRequestContext &context)
{
    context.response[0] = ThingSetStatusCode::content;
    context.encoder.encodePreamble();
    void *target;
    if (context.node->tryCastTo(ThingSetNodeType::encodable, &target)) {
        ThingSetBinaryEncodable *encodable = reinterpret_cast<ThingSetBinaryEncodable *>(target);
        if (encodable->encode(context.encoder)) {
            return context.encoder.getEncodedLength() + 1;
        }
    }
    context.response[0] = ThingSetStatusCode::unsupportedFormat;
    return 1;
}

int _ThingSetServer::handleFetch(ThingSetRequestContext &context)
{
    context.response[0] = ThingSetStatusCode::content;
    context.encoder.encodePreamble();
    if (context.decoder.decodeNull()) {
        // expect that this is a group
        void *target;
        if (context.node->tryCastTo(ThingSetNodeType::hasChildren, &target)) {
            ThingSetParentNode *parent = reinterpret_cast<ThingSetParentNode *>(target);
            if (context.useIds) {
                std::list<unsigned> ids;
                for (ThingSetNode *child : *parent) {
                    ids.push_back(child->getId());
                }
                context.encoder.encode(ids);
            }
            else {
                std::list<std::string_view> names;
                for (ThingSetNode *child : *parent) {
                    names.push_back(child->getName());
                }
                context.encoder.encode(names);
            }
            return context.encoder.getEncodedLength() + 1;
        }
        else {
            context.response[0] = ThingSetStatusCode::badRequest;
            return 1;
        }
    }
    else if (context.decoder.peekType() == ZCBOR_MAJOR_TYPE_LIST && context.encoder.encodeListStart()) {
        if (context.node == ThingSetRegistry::getMetadataNode() && context.decoder.decodeList([&context](size_t) {
                unsigned id;
                if (!context.decoder.decode(&id)) {
                    return false;
                }
                ThingSetNode *n;
                return ThingSetRegistry::findById(id, &n) && context.encoder.encodeMapStart()
                       && context.encoder.encode(MetadataNameId) && context.encoder.encode(n->getName())
                       && context.encoder.encode(MetadataTypeId) && context.encoder.encode(n->getType())
                       && context.encoder.encode(MetadataAccessId) && context.encoder.encode(n->getAccess())
                       && context.encoder.encodeMapEnd();
            })
            && context.encoder.encodeListEnd())
        {
            return context.encoder.getEncodedLength() + 1;
        }
        else {
            context.response[0] = ThingSetStatusCode::notFound;
            return 1;
        }
    }
    else {
        context.response[0] = ThingSetStatusCode::badRequest;
        return 1;
    }
}

int _ThingSetServer::handleUpdate(ThingSetRequestContext &context)
{
    void *target;
    if (!context.node->tryCastTo(ThingSetNodeType::hasChildren, &target)) {
        context.response[0] = ThingSetStatusCode::badRequest;
        return 1;
    }
    ThingSetParentNode *parent = reinterpret_cast<ThingSetParentNode *>(target);
    context.response[0] = ThingSetStatusCode::changed;
    context.encoder.encodePreamble();
    if (context.decoder.decodeMap<std::string>([&](auto &key) {
            // concoct full path to node
            std::string fullPath = std::string(context.path) + (context.path.length() > 1 ? "/" : "") + key;
            ThingSetNode *child;
            size_t index;
            if (!ThingSetRegistry::findByName(fullPath, &child, &index)) {
                context.response[0] = ThingSetStatusCode::notFound;
                return false;
            }
            if (!child->checkAccess(_access)) {
                context.response[0] = ThingSetStatusCode::forbidden;
                return false;
            }
            if (!child->tryCastTo(ThingSetNodeType::decodable, &target)) {
                context.response[0] = ThingSetStatusCode::badRequest;
                return false;
            }
            if (!parent->invokeCallback(child, ThingSetCallbackReason::willWrite)) {
                context.response[0] = ThingSetStatusCode::forbidden;
                return false;
            }
            ThingSetBinaryDecodable *decodable = reinterpret_cast<ThingSetBinaryDecodable *>(target);
            if (!decodable->decode(context.decoder)) {
                context.response[0] = ThingSetStatusCode::badRequest;
                return false;
            }
            // for now we ignore the return value here; what would returning false here mean?
            parent->invokeCallback(child, ThingSetCallbackReason::didWrite);
            return true;
        }))
    {
        context.encoder.encodePreamble();
        return context.encoder.getEncodedLength() + 1;
    }
    // just the error code
    return 1;
}

int _ThingSetServer::handleExec(ThingSetRequestContext &context)
{
    if (!context.node->checkAccess(_access)) {
        context.response[0] = ThingSetStatusCode::forbidden;
        return 1;
    }
    void *target;
    if (context.node->tryCastTo(ThingSetNodeType::function, &target)) {
        ThingSetInvocable *invocable = reinterpret_cast<ThingSetInvocable *>(target);
        context.encoder.encodePreamble();
        if (invocable->invoke(context.decoder, context.encoder)) {
            context.response[0] = ThingSetStatusCode::changed;
            return context.encoder.getEncodedLength() + 1;
        }
    }
    context.response[0] = ThingSetStatusCode::badRequest;
    return 1;
}

} // namespace ThingSet