/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "thingset++/ThingSetEncoder.hpp"

namespace ThingSet {

bool ThingSetEncoder::encode(ThingSetEncodable &value)
{
    return value.encode(*this);
}

bool ThingSetEncoder::encode(ThingSetEncodable *value)
{
    return value->encode(*this);
}

}