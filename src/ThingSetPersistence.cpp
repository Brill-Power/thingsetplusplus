/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifdef CONFIG_THINGSET_PLUS_PLUS_EEPROM

#include "thingset++/ThingSetPersistence.hpp"
#include "thingset++/ThingSetRegistry.hpp"
#include "thingset++/zephyr/StreamingZephyrEepromThingSetBinaryEncoder.hpp"
#include "thingset++/zephyr/StreamingZephyrEepromThingSetBinaryDecoder.hpp"

using namespace ThingSet::Zephyr;

namespace ThingSet {

ThingSetPersistence::ThingSetPersistence(const device *device) : _device(device)
{
}

bool ThingSetPersistence::load()
{
    StreamingZephyrEepromThingSetBinaryDecoder decoder(device, 0);
    return decoder.decodeMap<uint16_t>([&](uint16_t id) {
        ThingSetNode *node;
        if (!ThingSetRegistry::findById(id, &node)) {
            return false;
        }
        void *target;
        if (node->tryCastTo(ThingSetNodeType::decodable, &target)) {
            ThingSetBinaryDecodable *decodable = reinterpret_cast<ThingSetBinaryDecodable *>(target);
            return decodable->decode(decoder);
        }
        return false;
    });
}

bool ThingSetPersistence::save()
{
    StreamingZephyrEepromThingSetBinaryEncoder encoder(device, 0);
    // get count
    size_t count = 0;
    void *target;
    for (const ThingSetNode *node : ThingSetRegistry::nodesInSubset(Subset::persistence))
    {
        if (child->tryCastTo(ThingSetNodeType::encodable, &target))
        {
            count++;
        }
    }
    encoder.encodeMapStart(count);
    for (const ThingSetNode *node : ThingSetRegistry::nodesInSubset(Subset::persistence))
    {
        if (child->tryCastTo(ThingSetNodeType::encodable, &target))
        {
            ThingSetEncodable *encodable = reinterpret_cast<ThingSetEncodable *>(target);
            encoder.encode(std::make_pair(node->getId(), encodable));
        }
    }
    encoder.encodeMapEnd(count);
}

} // namespace ThingSet

#endif // CONFIG_THINGSET_PLUS_PLUS_EEPROM