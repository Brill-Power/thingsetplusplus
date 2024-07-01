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

    std::pair<uint8_t *, size_t> requestCallback(uint8_t *buffer, size_t len);

public:
    ThingSetServer(ThingSetServerTransport &transport);

    bool listen();

    template <typename T> bool publish(uint16_t id, T &value)
    {
        uint8_t buffer[1024];
        buffer[0] = THINGSET_BIN_REPORT;
        FixedSizeThingSetBinaryEncoder<8> encoder(buffer + 3, sizeof(buffer) - 3);
        encoder.encode(0); // fake subset ID
        size_t len = 3 + encoder.getEncodedLength();
        // reset encoder
        encoder = FixedSizeThingSetBinaryEncoder<8>(buffer + len, sizeof(buffer) - len);
        encoder.encodeMapStart() && encoder.encode(id) && encoder.encode(value) && encoder.encodeMapEnd();
        len = encoder.getEncodedLength() + 1; // TODO; this +1 should not be necessary
        buffer[1] = (uint8_t)len;
        buffer[2] = (uint8_t)(len >> 8);
        return _transport.publish(buffer, 3 + len);
    }
};

} // namespace ThingSet