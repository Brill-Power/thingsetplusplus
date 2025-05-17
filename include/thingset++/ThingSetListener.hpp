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
/// @tparam Identifier Type of identifier of a node (e.g. IP address, CAN ID).
template <typename Identifier>
class ThingSetListener
{
private:
    ThingSetSubscriptionTransport<Identifier> &_transport;

public:
    ThingSetListener(ThingSetSubscriptionTransport<Identifier> &transport) : _transport(transport)
    {}

    bool subscribe(std::function<void(const Identifier &, uint16_t &)> callback) {
        _transport.subscribe([&](const Identifier &identifier, ThingSetBinaryDecoder &decoder) {
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

class ThingSetListenerBuilder
{
public:
    template <typename Identifier>
    static ThingSetListener<Identifier> build(ThingSetSubscriptionTransport<Identifier> &transport) {
        return ThingSetListener<Identifier>(transport);
    }
};

}