/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifdef CONFIG_THINGSET_PLUS_PLUS_EEPROM

#include "thingset++/ThingSet.hpp"
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
    StreamingZephyrEepromThingSetBinaryDecoder decoder(_device, 0);

    if (decoder.isEmpty()) {
        return false;
    }

    if (decoder.getVersion() != CONFIG_THINGSET_PLUS_PLUS_EEPROM_DATA_VERSION) {
        LOG_WARN("EEPROM data version %d does not match expected version %d, "
                 "keeping default values",
                 decoder.getVersion(), CONFIG_THINGSET_PLUS_PLUS_EEPROM_DATA_VERSION);
        return false;
    }

    unsigned decoded = 0;
    unsigned lastId = 0;
    bool entryFailed = false;

    bool mapDecoded = decoder.decodeMap<uint16_t>([&](uint16_t id) {
        ThingSetNode *node = nullptr;
        void *target;
        bool ok;
        if (!ThingSetRegistry::findById(id, &node)) {
#ifdef CONFIG_THINGSET_PLUS_PLUS_EEPROM_SKIP_UNRECOGNISED
            LOG_WARN("Ignoring unknown persisted value 0x%x", id);
            ok = decoder.skip();
#else
            LOG_WARN("Unknown persisted value 0x%x, abandoning load", id);
            ok = false;
#endif
        }
        else if (node->tryCastTo(ThingSetNodeType::decodable, &target)) {
            ok = reinterpret_cast<ThingSetBinaryDecodable *>(target)->decode(decoder);
        }
        else {
#ifdef CONFIG_THINGSET_PLUS_PLUS_EEPROM_SKIP_UNRECOGNISED
            LOG_WARN("Ignoring persisted value 0x%x as it is not decodable", id);
            ok = decoder.skip();
#else
            LOG_WARN("Persisted value 0x%x is not decodable, abandoning load", id);
            ok = false;
#endif
        }
        if (!ok) {
            LOG_ERROR("Persisted value 0x%x (%s, type %s) failed after %u entries "
                      "(last good 0x%x), abandoning load",
                      id, node ? node->getName().data() : "not in registry",
                      node ? node->getType().c_str() : "unknown", decoded, lastId);
            entryFailed = true;
            return false;
        }
        decoded++;
        lastId = id;
        return true;
    });

    if (!mapDecoded) {
        if (!entryFailed) {
            LOG_ERROR("Persisted map could not be decoded after %u entries (last good 0x%x); "
                      "bad map header, key, or unterminated map",
                      decoded, lastId);
        }
        LOG_WARN("Persistence load abandoned after %u entries; values not yet decoded keep "
                 "their defaults",
                 decoded);
        return false;
    }

    if (!decoder.verify()) {
        LOG_WARN("Persisted data failed its CRC check; the %u decoded values were still applied",
                 decoded);
        return false;
    }

    LOG_INFO("Persistence load complete: %u values restored", decoded);
    return true;
}

bool ThingSetPersistence::save()
{
    StreamingZephyrEepromThingSetBinaryEncoder encoder(_device, 0);
    // get count
    size_t count = 0;
    void *target;
    for (ThingSetNode *node : ThingSetRegistry::nodesInSubset(Subset::persisted))
    {
        if (node->tryCastTo(ThingSetNodeType::encodable, &target))
        {
            count++;
        }
    }
    if (!encoder.encodeMapStart(count))
    {
        return false;
    }
    for (ThingSetNode *node : ThingSetRegistry::nodesInSubset(Subset::persisted))
    {
        if (node->tryCastTo(ThingSetNodeType::encodable, &target))
        {
            ThingSetEncodable *encodable = reinterpret_cast<ThingSetEncodable *>(target);
            if (!encoder.encode(std::make_pair(node->getId(), encodable)))
            {
                return false;
            }
        }
    }
    if (!encoder.encodeMapEnd(count))
    {
        return false;
    }
    return encoder.flush();
}

} // namespace ThingSet

#endif // CONFIG_THINGSET_PLUS_PLUS_EEPROM