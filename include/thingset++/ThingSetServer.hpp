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
#include "ThingSetRequestContext.hpp"
#include "ThingSetServerTransport.hpp"
#include "ThingSetStatus.hpp"

namespace ThingSet {

class ThingSetServer
{
private:
    ThingSetServerTransport &_transport;
    ThingSetAccess _access;

    int requestCallback(uint8_t *request, size_t requestLen, uint8_t *response, size_t responseLen);

public:
    ThingSetServer(ThingSetServerTransport &transport);

    bool listen();

    template <typename T> bool publish(uint16_t id, T &value)
    {
        uint8_t buffer[1024];
        buffer[0] = ThingSetRequestType::report;
        FixedDepthThingSetBinaryEncoder encoder(buffer + 3, 1024 - 3);
        encoder.encode(0); // fake subset ID
        size_t len = 3 + encoder.getEncodedLength();
        // reset encoder
        encoder = FixedDepthThingSetBinaryEncoder(buffer + len, 1024 - len);
        encoder.encodeMapStart() && encoder.encode(id) && encoder.encode(value) && encoder.encodeMapEnd();
        len = encoder.getEncodedLength() + 1; // TODO; this +1 should not be necessary
        buffer[1] = (uint8_t)len;
        buffer[2] = (uint8_t)(len >> 8);
        return _transport.publish(buffer, 3 + len);
    }

private:
    int handleGet(ThingSetRequestContext &context);
    int handleFetch(ThingSetRequestContext &context);
    int handleUpdate(ThingSetRequestContext &context);
    int handleExec(ThingSetRequestContext &context);
};

} // namespace ThingSet