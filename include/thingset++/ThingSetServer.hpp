/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "thingset++/ThingSetProperty.hpp"
#include "thingset++/ThingSetRequestContext.hpp"
#include "thingset++/ThingSetServerTransport.hpp"
#include "thingset++/ThingSetStatus.hpp"
#include "thingset++/StringLiteral.hpp"
#include "thingset++/internal/logging.hpp"

namespace ThingSet {

/// Specifies a type which is probably a ThingSet property.
template <typename T>
concept EncodableDecodableNode = std::is_base_of_v<ThingSetNode, T> &&
    std::is_base_of_v<ThingSetBinaryEncodable, T> &&
    std::is_base_of_v<ThingSetBinaryDecodable, T>;

class _ThingSetServer {
private:
    ThingSetAccess _access;

protected:
    _ThingSetServer();

public:
    virtual bool listen() = 0;

protected:
    int handleGet(ThingSetRequestContext &context);
    int handleFetch(ThingSetRequestContext &context);
    int handleUpdate(ThingSetRequestContext &context);
    int handleExec(ThingSetRequestContext &context);

    template <unsigned Id, unsigned ParentId, StringLiteral Name, ThingSetAccess Access, typename T, EncodableDecodableNode ... Property>
    bool encode(ThingSetBinaryEncoder &encoder, ThingSetProperty<Id, ParentId, Name, Access, T> &property, Property &... properties)
    {
        return encode(encoder, property) && encode(encoder, properties...);
    }

    template <unsigned Id, unsigned ParentId, StringLiteral Name, ThingSetAccess Access, typename T>
    bool encode(ThingSetBinaryEncoder &encoder, ThingSetProperty<Id, ParentId, Name, Access, T> &property)
    {
        return encoder.encode(property.getId()) && encoder.encode(property.getValue());
    }
};

/// @brief Core server implementation.
/// @tparam Identifier Type of client identifier
/// @tparam Size Size of broadcast message frames
/// @tparam Encoder Type of streaming encoder
template <typename Identifier, size_t Size, StreamingBinaryEncoder<Size> Encoder>
class ThingSetServer : public _ThingSetServer
{
private:
    ThingSetServerTransport<Identifier, Size, Encoder> &_transport;

public:
    ThingSetServer(ThingSetServerTransport<Identifier, Size, Encoder> &transport)
    : _ThingSetServer(), _transport(transport)
    {}

    bool listen() override
    {
        return _transport.listen(
            [this](auto sender, auto req, auto reql, auto res, auto resl) { return requestCallback(sender, req, reql, res, resl); });
    }

    /// @brief Broadcasts one or more properties as a report.
    /// @tparam ...Property The types of the properties.
    /// @param ...properties The properties to publish.
    /// @return True if publishing succeeded.
    template <EncodableDecodableNode ... Property> bool publish(Property &...properties)
    {
        Encoder encoder = _transport.getPublishingEncoder();
        if (!encoder.encode(0) || // fake subset ID
            !encoder.encodeMapStart(sizeof...(properties)) ||
            !encode(encoder, properties...) ||
            !encoder.encodeMapEnd()) {
            return false;
        }
        return encoder.flush();
    }

private:
    int requestCallback(Identifier &identifier, uint8_t *request, size_t requestLen, uint8_t *response, size_t responseLen)
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
               // LOG_SMART("Handling get for node ", context.node->getName(), " from ", identifier);
                return handleGet(context);
            case ThingSetRequestType::fetch:
               // LOG_SMART("Handling fetch for node ", context.node->getName(), " from ", identifier);
                return handleFetch(context);
            case ThingSetRequestType::update:
               // LOG_SMART("Handling update for node ", context.node->getName(), " from ", identifier);
                return handleUpdate(context);
            case ThingSetRequestType::exec:
               // LOG_SMART("Handling exec for node ", context.node->getName(), " from ", identifier);
                return handleExec(context);
            default:
                break;
        }
        response[0] = ThingSetStatusCode::notImplemented;
        return 1;
    }
};

class ThingSetServerBuilder {
public:
    template <typename Identifier, size_t Size, StreamingBinaryEncoder<Size> Encoder>
    static ThingSetServer<Identifier, Size, Encoder> build(ThingSetServerTransport<Identifier, Size, Encoder> &transport) {
        return ThingSetServer<Identifier, Size, Encoder>(transport);
    }
};

} // namespace ThingSet