/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "thingset++/ThingSetDecoder.hpp"

namespace ThingSet {

bool ThingSetDecoder::decodeKey(std::optional<uint32_t> &id, std::optional<std::string> &name)
{
    uint32_t i;
    std::string n;
    if (decode(&i)) {
        id = i;
        return true;
    }
    else if (decode(&n)) {
        name = n;
        return true;
    } else {
        return false;
    }
}

bool ThingSetDecoder::decodeList(std::function<bool(size_t)> callback)
{
    if (!decodeListStart()) {
        return false;
    }

    size_t index = 0;
    while (isInList()) {
        if (!callback(index++)) {
            return false;
        }
    }

    return decodeListEnd();
}

bool ThingSetDecoder::skipUntil(ThingSetEncodedNodeType sought)
{
    ThingSetEncodedNodeType type;
    while ((type = this->peekType()) != sought) {
        this->skip();
    };
    return sought == type;
}

} // namespace ThingSet
