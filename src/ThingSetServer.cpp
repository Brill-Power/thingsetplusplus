/*
 * Copyright (c) 2024 Brill Power.
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

ThingSetServer::ThingSetServer(ThingSetServerTransport &transport)
    : _transport(transport), _access(ThingSetAccess::userRead | ThingSetAccess::userWrite)
{}

int ThingSetServer::requestCallback(uint8_t *request, size_t requestLen, uint8_t *response, size_t responseLen)
{
    ThingSetRequestContext context(request, requestLen, response, responseLen);

    uint16_t id;
    if (context.decoder.decode(&context.path)) {
        if (!ThingSetRegistry::findByName(context.path, &context.node, &context.index)) {
            response[0] = ThingSetStatusCode::notFound;
            return 1;
        }
    }
    else if (context.decoder.decode(&id)) {
        if (!ThingSetRegistry::findById(id, &context.node)) {
            response[0] = ThingSetStatusCode::notFound;
            return 1;
        }
        context.useIds = true;
    }
    else {
        // fail
        response[0] = ThingSetStatusCode::badRequest;
        return 1;
    }
    void *target;
    if (context.node->tryCastTo(ThingSetNodeType::requestHandler, &target)) {
        ThingSetCustomRequestHandler *handler = reinterpret_cast<ThingSetCustomRequestHandler *>(target);
        int result = handler->handleRequest(context);
        if (result != 0) {
            return result;
        }
    }
    switch (request[0]) {
        case ThingSetRequestType::get:
            return handleGet(context);
        case ThingSetRequestType::fetch:
            return handleFetch(context);
        case ThingSetRequestType::update:
            return handleUpdate(context);
        case ThingSetRequestType::exec:
            return handleExec(context);
        default:
            break;
    }
    response[0] = ThingSetStatusCode::notImplemented;
    return 1;
}

int ThingSetServer::handleGet(ThingSetRequestContext &context)
{
    context.response[0] = ThingSetStatusCode::content;
    context.encoder.encodeNull();
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

int ThingSetServer::handleFetch(ThingSetRequestContext &context)
{
    context.response[0] = ThingSetStatusCode::content;
    context.encoder.encodeNull();
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
                       && context.encoder.encode("name") && context.encoder.encode(n->getName())
                       && context.encoder.encode("type") && context.encoder.encode(n->getType())
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

int ThingSetServer::handleUpdate(ThingSetRequestContext &context)
{
    void *target;
    if (!context.node->tryCastTo(ThingSetNodeType::hasChildren, &target)) {
        context.response[0] = ThingSetStatusCode::badRequest;
        return 1;
    }
    ThingSetParentNode *parent = reinterpret_cast<ThingSetParentNode *>(target);
    context.response[0] = ThingSetStatusCode::changed;
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
        context.encoder.encodeNull();
        return context.encoder.getEncodedLength() + 1;
    }
    // just the error code
    return 1;
}

int ThingSetServer::handleExec(ThingSetRequestContext &context)
{
    if (!context.node->checkAccess(_access)) {
        context.response[0] = ThingSetStatusCode::forbidden;
        return 1;
    }
    void *target;
    if (context.node->tryCastTo(ThingSetNodeType::function, &target)) {
        ThingSetInvocable *invocable = reinterpret_cast<ThingSetInvocable *>(target);
        if (invocable->invoke(context.decoder, context.encoder)) {
            context.response[0] = ThingSetStatusCode::changed;
            return context.encoder.getEncodedLength() + 1;
        }
    }
    context.response[0] = ThingSetStatusCode::badRequest;
    return 1;
}

bool ThingSetServer::listen()
{
    return _transport.listen(
        [this](auto req, auto reql, auto res, auto resl) { return requestCallback(req, reql, res, resl); });
}

} // namespace ThingSet