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

class _ThingSetServer
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

    int handleRequest(ThingSetRequestContext &context);

protected:
    int handleBinaryRequest(uint8_t *request, size_t requestLen, uint8_t *response, size_t responseLen);
    int handleTextRequest(uint8_t *request, size_t requestLen, uint8_t *response, size_t responseLen);
    virtual bool handleForward(ThingSetRequestContext &context) = 0;

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

class ThingSetForwarder
{
protected:
    virtual bool tryForward(ThingSetRequestContext &context) = 0;
};

class DefaultForwarder : public ThingSetForwarder
{
protected:
    inline bool tryForward(ThingSetRequestContext &) override
    {
        return false;
    }
};

/// @brief Core server implementation.
/// @tparam Identifier Type of client identifier
/// @tparam Size Size of broadcast message frames
/// @tparam Encoder Type of streaming encoder
template <typename Identifier, size_t Size, StreamingBinaryEncoder<Size> Encoder, typename Forwarder = DefaultForwarder>
    requires std::is_base_of_v<ThingSetForwarder, Forwarder>
class ThingSetServer : public _ThingSetServer, public Forwarder
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

protected:
    bool handleForward(ThingSetRequestContext &context) override
    {
        return this->tryForward(context);
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
    template <typename Identifier, size_t Size, StreamingBinaryEncoder<Size> Encoder>
    static ThingSetServer<Identifier, Size, Encoder>
    build(ThingSetServerTransport<Identifier, Size, Encoder> &transport)
    {
        return ThingSetServer<Identifier, Size, Encoder>(transport);
    }
};

} // namespace ThingSet