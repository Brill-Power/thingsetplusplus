/*
 * Copyright (c) 2024 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */
#pragma once

#include "ThingSetBinaryDecoder.hpp"
#include "ThingSetBinaryEncoder.hpp"
#include "ThingSetClientTransport.hpp"
#include "ThingSetStatus.hpp"

namespace ThingSet {

class ThingSetClient
{
private:
    ThingSetClientTransport &_transport;

public:
    ThingSetClient(ThingSetClientTransport &transport);

    void connect();

    template <typename T> bool get(uint16_t id, T &result)
    {
        uint8_t buffer[1024];
        buffer[0] = ThingSetRequestType::get;
        FixedDepthThingSetBinaryEncoder encoder(buffer + 1, sizeof(buffer) - 1);
        if (!encoder.encode(id)) {
            return false;
        }
        if (!_transport.write(buffer, 1 + encoder.getEncodedLength())) {
            return false;
        }
        int read = _transport.read(buffer, 1024);
        if (read > 1) {
            FixedDepthThingSetBinaryDecoder decoder(buffer + 1, read - 1);
            uint32_t id;
            decoder.decode(&id);
            return decoder.decode(result);
        }
        return false;
    }

    template <typename T> void subscribe(T *items)
    {
        _transport.subscribe([items](auto buffer, auto len) {
            FixedDepthThingSetBinaryDecoder decoder(buffer, len);
            uint16_t id;
            decoder.decode(&id); // subset ID
            size_t pos = decoder.getDecodedLength();
            decoder = FixedDepthThingSetBinaryDecoder(buffer + pos, len - pos);
            if (decoder.skipUntil(ZCBOR_MAJOR_TYPE_MAP)) {
                decoder.template decodeMap<uint16_t>([&decoder, items](uint16_t &) { return decoder.decode(items); });
            }
        });
    }
};

} // namespace ThingSet