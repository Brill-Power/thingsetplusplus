/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "thingset++/ThingSetDecoder.hpp"

namespace ThingSet {

bool ThingSetDecoder::decodeKey(uint32_t &id, std::string &name)
{
    id = UINT32_MAX;
    name = "";
    if (decode(&id)) {
        return true;
    }
    else if (decode(&name)) {
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

} // namespace ThingSet
