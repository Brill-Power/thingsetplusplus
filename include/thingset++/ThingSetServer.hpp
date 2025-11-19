/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "thingset++/Eui.hpp"
#include "thingset++/StringLiteral.hpp"
#include "thingset++/ThingSetProperty.hpp"
#include "thingset++/ThingSetRequestContext.hpp"
#include "thingset++/ThingSetServerTransport.hpp"
#include "thingset++/ThingSetStatus.hpp"
#include "thingset++/internal/logging.hpp"

namespace ThingSet {

/// Specifies a type which is probably a ThingSet property.
template <typename T>
concept EncodableNode = std::is_base_of_v<ThingSetNode, T> && std::is_base_of_v<ThingSetEncodable, T>;

class ThingSetForwarder
{
public:
    virtual int handleForward(ThingSetRequestContext &context, uint8_t *request, size_t requestLen, uint8_t *response, size_t responseSize) = 0;
};

class _ThingSetServer
{
private:
    ThingSetAccess _access;
    ThingSetForwarder *_forwarder;

protected:
    _ThingSetServer(ThingSetForwarder *forwarder);

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
#ifdef ENABLE_TEXT_MODE
    int handleTextRequest(uint8_t *request, size_t requestLen, uint8_t *response, size_t responseSize);
#endif // ENABLE_TEXT_MODE

    template <typename T, ThingSetAccess Access, typename SubsetType, SubsetType Subset,
              EncodableNode... Property>
    bool encode(ThingSetEncoder &encoder, ThingSetProperty<T, Access, SubsetType, Subset> &property,
                Property &...properties)
    {
        return encode(encoder, property) && encode(encoder, properties...);
    }

    template <typename T, ThingSetAccess Access, typename SubsetType, SubsetType Subset>
    bool encode(ThingSetEncoder &encoder, ThingSetProperty<T, Access, SubsetType, Subset> &property)
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
    ThingSetServer(ThingSetServerTransport<Identifier, Size, Encoder> &transport) : ThingSetServer(transport, nullptr)
    {}

    ThingSetServer(ThingSetServerTransport<Identifier, Size, Encoder> &transport, ThingSetForwarder *forwarder)
        : _ThingSetServer(forwarder), _transport(transport)
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
    template <EncodableNode... Property> bool publish(Property &...properties)
    {
#ifdef ENABLE_ENHANCED_REPORTING
        bool enhanced = true;
#else
        bool enhanced = false;
#endif
        Encoder encoder = _transport.getPublishingEncoder(enhanced);

        if (enhanced) {
            if (!encoder.encode(ThingSet::Eui::getValue())) {
                return false;
            }
        }

        if (!encoder.encode(0)) { // fake subset ID
            return false;
        }

        if (!encoder.encodeMapStart(sizeof...(properties)) || !encode(encoder, properties...)
            || !encoder.encodeMapEnd())
        {
            return false;
        }
        return encoder.flush();
    }

private:
    int requestCallback(Identifier &, uint8_t *request, size_t requestLen, uint8_t *response, size_t responseLen)
    {
        if ((request[0] >= ThingSetBinaryRequestType::get && request[0] <= ThingSetBinaryRequestType::update)
        || request[0] == ThingSetBinaryRequestType::forward)
        {
            return handleBinaryRequest(request, requestLen, response, responseLen);
        }
#ifdef ENABLE_TEXT_MODE
        else if (request[0] >= (uint8_t)ThingSetTextRequestType::exec && request[0] <= (uint8_t)ThingSetTextRequestType::desire)
        {
            return handleTextRequest(request, requestLen, response, responseLen);
        }
#endif // ENABLE_TEXT_MODE
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
    build(ThingSetServerTransport<Identifier, Size, Encoder> &transport, ThingSetForwarder *forwarder = nullptr)
    {
        return ThingSetServer<Identifier, Size, Encoder>(transport, forwarder);
    }
};

} // namespace ThingSet