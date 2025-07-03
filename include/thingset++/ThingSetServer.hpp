/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "thingset++/StringLiteral.hpp"
#include "thingset++/ThingSetProperty.hpp"
#include "thingset++/ThingSetRequestContext.hpp"
#include "thingset++/ThingSetServerTransport.hpp"
#include "thingset++/ThingSetStatus.hpp"
#include "thingset++/internal/logging.hpp"

namespace ThingSet {

/// Specifies a type which is probably a ThingSet property.
template <typename T>
concept EncodableDecodableNode = std::is_base_of_v<ThingSetNode, T> && std::is_base_of_v<ThingSetEncodable, T>
                                 && std::is_base_of_v<ThingSetDecodable, T>;

class ThingSetForwarder
{
protected:
    virtual int handleForward(ThingSetRequestContext &context, uint8_t *request, size_t requestLen, uint8_t *response, size_t responseSize) = 0;

    static bool tryGetNodeId(const std::string &path, std::string &nodeId);
};

class _ThingSetServer : protected virtual ThingSetForwarder
{
private:
    ThingSetAccess _access;

protected:
    _ThingSetServer();

public:
    virtual bool listen() = 0;

private:
    int handleGet(ThingSetRequestContext &context);
    int handleFetch(ThingSetRequestContext &context);
    int handleUpdate(ThingSetRequestContext &context);
    int handleExec(ThingSetRequestContext &context);

    int handleRequest(ThingSetRequestContext &context, uint8_t *request, size_t requestLen, uint8_t *response, size_t responseSize);

protected:
    int handleBinaryRequest(uint8_t *request, size_t requestLen, uint8_t *response, size_t responseSize);
    int handleTextRequest(uint8_t *request, size_t requestLen, uint8_t *response, size_t responseSize);

    template <unsigned Id, unsigned ParentId, StringLiteral Name, ThingSetAccess Access, typename T,
              EncodableDecodableNode... Property>
    bool encode(ThingSetEncoder &encoder, ThingSetProperty<Id, ParentId, Name, Access, T> &property,
                Property &...properties)
    {
        return encode(encoder, property) && encode(encoder, properties...);
    }

    template <unsigned Id, unsigned ParentId, StringLiteral Name, ThingSetAccess Access, typename T>
    bool encode(ThingSetEncoder &encoder, ThingSetProperty<Id, ParentId, Name, Access, T> &property)
    {
        return encoder.encode(property.getId()) && encoder.encode(property.getValue());
    }
};

class DefaultForwarder : protected virtual ThingSetForwarder
{
protected:
    inline int handleForward(ThingSetRequestContext &context, uint8_t *, size_t, uint8_t *, size_t) override
    {
        context.setStatus(ThingSetStatusCode::notAGateway);
        return context.getHeaderLength();
    }
};

/// @brief Core server implementation.
/// @tparam Identifier Type of client identifier
/// @tparam Size Size of broadcast message frames
/// @tparam Encoder Type of streaming encoder
template <typename Identifier, size_t Size, StreamingBinaryEncoder<Size> Encoder, typename Forwarder = DefaultForwarder>
    requires std::is_base_of_v<ThingSetForwarder, Forwarder>
class ThingSetServer : public _ThingSetServer, protected virtual Forwarder
{
private:
    ThingSetServerTransport<Identifier, Size, Encoder> &_transport;

public:
    ThingSetServer(ThingSetServerTransport<Identifier, Size, Encoder> &transport)
        : _ThingSetServer(), _transport(transport)
    {}

    bool listen() override
    {
        return _transport.listen([this](auto sender, auto req, auto reql, auto res, auto resl) {
            return requestCallback(sender, req, reql, res, resl);
        });
    }

    /// @brief Broadcasts one or more properties as a report.
    /// @tparam ...Property The types of the properties.
    /// @param ...properties The properties to publish.
    /// @return True if publishing succeeded.
    template <EncodableDecodableNode... Property> bool publish(Property &...properties)
    {
        Encoder encoder = _transport.getPublishingEncoder();
        if (!encoder.encode(0) || // fake subset ID
            !encoder.encodeMapStart(sizeof...(properties)) || !encode(encoder, properties...)
            || !encoder.encodeMapEnd())
        {
            return false;
        }
        return encoder.flush();
    }

private:
    int requestCallback(Identifier &, uint8_t *request, size_t requestLen, uint8_t *response, size_t responseLen)
    {
        if (request[0] >= ThingSetBinaryRequestType::get && request[0] <= ThingSetBinaryRequestType::report)
        {
            return handleBinaryRequest(request, requestLen, response, responseLen);
        }
        else if (request[0] >= (uint8_t)ThingSetTextRequestType::exec && request[0] <= (uint8_t)ThingSetTextRequestType::desire)
        {
            return handleTextRequest(request, requestLen, response, responseLen);
        }
        else
        {
            response[0] = ThingSetStatusCode::badRequest;
            return 1;
        }
    }
};

class ThingSetServerBuilder
{
public:
    template <typename Identifier, size_t Size, StreamingBinaryEncoder<Size> Encoder, typename Forwarder = DefaultForwarder>
        requires std::is_base_of_v<ThingSetForwarder, Forwarder>
    static ThingSetServer<Identifier, Size, Encoder>
    build(ThingSetServerTransport<Identifier, Size, Encoder> &transport)
    {
        return ThingSetServer<Identifier, Size, Encoder, Forwarder>(transport);
    }
};

} // namespace ThingSet