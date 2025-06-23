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

int _ThingSetServer::handleBinaryRequest(uint8_t *request, size_t requestLen, uint8_t *response, size_t responseLen)
{
    ThingSetBinaryRequestContext context(request, requestLen, response, responseLen);
    return handleRequest(context);
}

int _ThingSetServer::handleTextRequest(uint8_t *request, size_t requestLen, uint8_t *response, size_t responseLen)
{
    ThingSetTextRequestContext context(request, requestLen, response, responseLen);
    return handleRequest(context);
}

int _ThingSetServer::handleRequest(ThingSetRequestContext &context)
{
    if (!context.hasValidEndpoint())
    {
        // fail
        context.setStatus(ThingSetStatusCode::badRequest);
        return context.getHeaderLength();
    }
    if (context.useIds())
    {
        if (!ThingSetRegistry::findById(context.id(), &context.node)) {
            context.setStatus(ThingSetStatusCode::notFound);
            return context.getHeaderLength();
        }
    }
    else if (!ThingSetRegistry::findByName(context.path(), &context.node, &context.index)) {
        context.setStatus(ThingSetStatusCode::notFound);
        return context.getHeaderLength();
    }

    void *target;
    if (context.node->tryCastTo(ThingSetNodeType::requestHandler, &target)) {
        ThingSetCustomRequestHandler *handler = reinterpret_cast<ThingSetCustomRequestHandler *>(target);
        int result = handler->handleRequest(context);
        if (result != 0) {
            return result;
        }
    }
    if (context.isGet()) {
        return handleGet(context);
    } else if (context.isFetch()) {
        return handleFetch(context);
    } else if (context.isUpdate()) {
        return handleUpdate(context);
    } else if (context.isExec()) {
        return handleExec(context);
    }
    context.setStatus(ThingSetStatusCode::notImplemented);
    return context.getHeaderLength();
}

int _ThingSetServer::handleGet(ThingSetRequestContext &context)
{
    context.setStatus(ThingSetStatusCode::content);
    context.encoder().encodePreamble();
    void *target;
    if (context.node->tryCastTo(ThingSetNodeType::encodable, &target)) {
        ThingSetEncodable *encodable = reinterpret_cast<ThingSetEncodable *>(target);
        if (encodable->encode(context.encoder())) {
            return context.encoder().getEncodedLength() + context.getHeaderLength();
        }
    }
    else if (context.node->tryCastTo(ThingSetNodeType::hasChildren, &target)) {
        ThingSetParentNode *parent = reinterpret_cast<ThingSetParentNode *>(target);
        // first get a count of encodable child nodes
        std::list<unsigned> ids;
        for (ThingSetNode *child : *parent)
        {
            if (child->tryCastTo(ThingSetNodeType::encodable, &target))
            {
                ids.push_back(child->getId());
            }
        }
        context.encoder().encodeMapStart(ids.size());
        for (ThingSetNode *child : *parent)
        {
            if (child->tryCastTo(ThingSetNodeType::encodable, &target))
            {
                ThingSetEncodable *encodable = reinterpret_cast<ThingSetEncodable *>(target);
                if (context.useIds())
                {
                    context.encoder().encode(std::make_pair(child->getId(), encodable));
                }
                else
                {
                    context.encoder().encode(std::make_pair(child->getName(), encodable));
                }
            }
        }
        context.encoder().encodeMapEnd();
        return context.encoder().getEncodedLength() + context.getHeaderLength();
    }
    context.setStatus(ThingSetStatusCode::unsupportedFormat);
    return context.getHeaderLength();
}

int _ThingSetServer::handleFetch(ThingSetRequestContext &context)
{
    context.setStatus(ThingSetStatusCode::content);
    context.encoder().encodePreamble();
    if (context.decoder().decodeNull()) {
        // expect that this is a group
        void *target;
        if (context.node->tryCastTo(ThingSetNodeType::hasChildren, &target)) {
            ThingSetParentNode *parent = reinterpret_cast<ThingSetParentNode *>(target);
            if (context.useIds()) {
                std::list<unsigned> ids;
                for (ThingSetNode *child : *parent) {
                    ids.push_back(child->getId());
                }
                context.encoder().encode(ids);
            }
            else {
                std::list<std::string_view> names;
                for (ThingSetNode *child : *parent) {
                    names.push_back(child->getName());
                }
                context.encoder().encode(names);
            }
            return context.encoder().getEncodedLength() + context.getHeaderLength();
        }
        else {
            context.setStatus(ThingSetStatusCode::badRequest);
            return context.getHeaderLength();
        }
    }
    else if (context.decoder().peekType() == ThingSetEncodedNodeType::list && context.encoder().encodeListStart()) {
        if (context.node == ThingSetRegistry::getMetadataNode() && context.decoder().decodeList([&context](size_t) {
                unsigned id;
                if (!context.decoder().decode(&id)) {
                    return false;
                }
                ThingSetNode *n;
                return ThingSetRegistry::findById(id, &n) && context.encoder().encodeMapStart()
                       && context.encoder().encode(MetadataNameId) && context.encoder().encode(n->getName())
                       && context.encoder().encode(MetadataTypeId) && context.encoder().encode(n->getType())
                       && context.encoder().encode(MetadataAccessId) && context.encoder().encode(n->getAccess())
                       && context.encoder().encodeMapEnd();
            })
            && context.encoder().encodeListEnd())
        {
            return context.encoder().getEncodedLength() + context.getHeaderLength();
        }
        else {
            context.setStatus(ThingSetStatusCode::notFound);
            return context.getHeaderLength();
        }
    }
    else {
        context.setStatus(ThingSetStatusCode::badRequest);
        return context.getHeaderLength();
    }
}

int _ThingSetServer::handleUpdate(ThingSetRequestContext &context)
{
    void *target;
    if (!context.node->tryCastTo(ThingSetNodeType::hasChildren, &target)) {
        context.setStatus(ThingSetStatusCode::badRequest);
        return context.getHeaderLength();
    }
    ThingSetParentNode *parent = reinterpret_cast<ThingSetParentNode *>(target);
    context.setStatus(ThingSetStatusCode::changed);
    context.encoder().encodePreamble();
    if (context.decoder().decodeMap<std::string>([&](auto &key) {
        ThingSetNode *child;
        size_t index;
        if (!parent->findByName(key, &child, &index)) {
            context.setStatus(ThingSetStatusCode::notFound);
            return false;
        }
        if (!child->checkAccess(_access)) {
            context.setStatus(ThingSetStatusCode::forbidden);
            return false;
        }
        if (!child->tryCastTo(ThingSetNodeType::decodable, &target)) {
            context.setStatus(ThingSetStatusCode::badRequest);
            return false;
        }
        if (!parent->invokeCallback(child, ThingSetCallbackReason::willWrite)) {
            context.setStatus(ThingSetStatusCode::forbidden);
            return false;
        }
        ThingSetDecodable *decodable = reinterpret_cast<ThingSetDecodable *>(target);
        if (!decodable->decode(context.decoder())) {
            context.setStatus(ThingSetStatusCode::badRequest);
            return false;
        }
        // for now we ignore the return value here; what would returning false here mean?
        parent->invokeCallback(child, ThingSetCallbackReason::didWrite);
        return true;
    }))
    {
        context.encoder().encodePreamble();
        return context.encoder().getEncodedLength() + context.getHeaderLength();
    }
    // just the error code
    return context.getHeaderLength();
}

int _ThingSetServer::handleExec(ThingSetRequestContext &context)
{
    if (!context.node->checkAccess(_access)) {
        context.setStatus(ThingSetStatusCode::forbidden);
        return context.getHeaderLength();
    }
    void *target;
    if (context.node->tryCastTo(ThingSetNodeType::function, &target)) {
        ThingSetInvocable *invocable = reinterpret_cast<ThingSetInvocable *>(target);
        context.encoder().encodePreamble();
        if (invocable->invoke(context.decoder(), context.encoder())) {
            context.setStatus(ThingSetStatusCode::changed);
            return context.encoder().getEncodedLength() + context.getHeaderLength();
        }
    }
    context.setStatus(ThingSetStatusCode::badRequest);
    return context.getHeaderLength();
}

} // namespace ThingSet