/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "ThingSetAccess.hpp"
#include "ThingSetBinaryDecoder.hpp"
#include "ThingSetBinaryEncoder.hpp"
#include "ThingSetNode.hpp"
#include "ThingSetProperty.hpp"
#include "ThingSetRequestContext.hpp"
#include "ThingSetServerTransport.hpp"
#include "ThingSetStatus.hpp"
#include "StringLiteral.hpp"

namespace ThingSet {

class _ThingSetServer
{
private:
    ThingSetAccess _access;

protected:
    _ThingSetServer() : _access(ThingSetAccess::userRead | ThingSetAccess::userWrite)
    {}

    int requestCallback(uint8_t *request, size_t requestLen, uint8_t *response, size_t responseLen);

private:
    int handleGet(ThingSetRequestContext &context);
    int handleFetch(ThingSetRequestContext &context);
    int handleUpdate(ThingSetRequestContext &context);
    int handleExec(ThingSetRequestContext &context);

    template <unsigned Id, unsigned ParentId, StringLiteral Name, ThingSetAccess Access, typename T, EncodableDecodableNode ... Property>
    bool encode(ThingSetBinaryEncoder &encoder, ThingSetProperty<Id, ParentId, Name, Access, T> &property, Property &... properties)
    {
        return encode(encoder, property) && encode(encoder, properties...);
    }

    template <unsigned Id, unsigned ParentId, StringLiteral Name, ThingSetAccess Access, typename T> bool encode(ThingSetBinaryEncoder &encoder, ThingSetProperty<Id, ParentId, Name, Access, T> &property)
    {
        return encoder.encode(property.getId()) && encoder.encode(property.getValue());
    }
};

/// @brief Core server implementation.
template <size_t StreamingFrameSize, typename StreamingEncoder, typename Transport>
    requires std::is_base_of_v<ThingSetServerTransport<StreamingFrameSize, StreamingEncoder>, Transport>
class ThingSetServer : public _ThingSetServer
{
private:
    Transport &_transport;

public:
    ThingSetServer(Transport &transport) : _ThingSetServer(), _transport(transport)
    {}

    bool listen() {
        return _transport.listen(
            [this](auto req, auto reql, auto res, auto resl) { return requestCallback(req, reql, res, resl); });
    }

    /// @brief Broadcasts one or more properties as a report.
    /// @tparam ...Property The types of the properties.
    /// @param ...properties The properties to publish.
    /// @return True if publishing succeeded.
    template <EncodableDecodableNode ... Property> bool publish(Property &...properties)
    {
        uint8_t buffer[1024];
        buffer[0] = ThingSetRequestType::report;
        FixedDepthThingSetBinaryEncoder encoder(buffer + 3, 1024 - 3, 2);
        if (!encoder.encode(0) || // fake subset ID
            !encoder.encodeMapStart() ||
            !encode(encoder, properties...) ||
            !encoder.encodeMapEnd()) {
            return false;
        }
        size_t len = encoder.getEncodedLength();
        buffer[1] = (uint8_t)len;
        buffer[2] = (uint8_t)(len >> 8);
        return _transport.publish(buffer, 3 + len);
    }
};

} // namespace ThingSet