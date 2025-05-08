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

/// Specifies a type which is probably a ThingSet property.
template <typename T>
concept EncodableDecodableNode = std::is_base_of_v<ThingSetNode, T> &&
    std::is_base_of_v<ThingSetBinaryEncodable, T> &&
    std::is_base_of_v<ThingSetBinaryDecodable, T>;

/// @brief Core server implementation.
class ThingSetServer
{
private:
    ThingSetServerTransport &_transport;
    ThingSetAccess _access;

    int requestCallback(uint8_t *request, size_t requestLen, uint8_t *response, size_t responseLen);

public:
    ThingSetServer(ThingSetServerTransport &transport);

    bool listen();

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

    template <unsigned Id, unsigned ParentId, StringLiteral Name, ThingSetAccess Access, typename T>
    bool encode(ThingSetBinaryEncoder &encoder, ThingSetProperty<Id, ParentId, Name, Access, T> &property)
    {
        return encoder.encode(property.getId()) && encoder.encode(property.getValue());
    }
};

} // namespace ThingSet