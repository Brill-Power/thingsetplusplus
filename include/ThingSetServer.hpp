/*
 * Copyright (c) 2024 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */
#pragma once

#include "ThingSetServerTransport.hpp"
#include "internal/thingset.h"
#include <ThingSetBinaryEncoder.hpp>

namespace ThingSet {

class ThingSetServer
{
private:
    ThingSetServerTransport &_transport;

    int requestCallback(uint8_t *request, size_t requestLen, uint8_t *response, size_t responseLen);

public:
    ThingSetServer(ThingSetServerTransport &transport);

    bool listen();

    template <typename T> bool publish(uint16_t id, T &value)
    {
        uint8_t buffer[1024];
        buffer[0] = THINGSET_BIN_REPORT;
        FixedSizeThingSetBinaryEncoder encoder(buffer + 3, 1024 - 3);
        encoder.encode(0); // fake subset ID
        size_t len = 3 + encoder.getEncodedLength();
        // reset encoder
        encoder = FixedSizeThingSetBinaryEncoder(buffer + len, 1024 - len);
        encoder.encodeMapStart() && encoder.encode(id) && encoder.encode(value) && encoder.encodeMapEnd();
        len = encoder.getEncodedLength() + 1; // TODO; this +1 should not be necessary
        buffer[1] = (uint8_t)len;
        buffer[2] = (uint8_t)(len >> 8);
        return _transport.publish(buffer, 3 + len);
    }
};

} // namespace ThingSet