/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "thingset++/ThingSetBinaryDecoder.hpp"
#include "thingset++/ThingSetBinaryEncoder.hpp"
#include "thingset++/ThingSetSubscriptionTransport.hpp"

namespace ThingSet {

/// @brief ThingSet broadcast listener.
template <typename Identifier>
class _ThingSetListener
{
private:
    ThingSetSubscriptionTransport<Identifier> &_transport;

public:
    _ThingSetListener(ThingSetSubscriptionTransport<Identifier> &transport) : _transport(transport)
    {}

    bool connect() {
        return _transport.connect();
    }

    bool subscribe(std::function<void(Identifier &, uint16_t &)> callback) {
        _transport.subscribe([&](Identifier &identifier, uint8_t *buffer, size_t length) {
            DefaultFixedDepthThingSetBinaryDecoder decoder(buffer, length, 2);
            uint16_t subsetId;
            if (!decoder.decode(&subsetId)) {
                return;
            }

            if (!decoder.decodeMap<uint16_t>([&](uint16_t id) {
                ThingSetNode *node;
                if (!ThingSetRegistry::findById(id, &node)) {
                    return false;
                }
                void *target;
                if (node->tryCastTo(ThingSetNodeType::decodable, &target)) {
                    ThingSetBinaryDecodable *decodable = reinterpret_cast<ThingSetBinaryDecodable *>(target);
                    if (decodable->decode(decoder)) {
                        callback(identifier, id);
                        return true;
                    } else {
                        return false;
                    }
                }
                return false;
            })) {
                return;
            }
        });
        return true;
    }
};

class ThingSetListener
{
public:
    template <typename Identifier>
    static _ThingSetListener<Identifier> build(ThingSetSubscriptionTransport<Identifier> &transport) {
        return _ThingSetListener<Identifier>(transport);
    }
};

}