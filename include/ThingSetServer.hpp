/*
 * Copyright (c) 2024 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */
#pragma once

#include "ThingSetServerTransport.hpp"
#include "internal/thingset.h"

namespace ThingSet {

class ThingSetServer
{
private:
    ThingSetServerTransport &_transport;

    std::pair<uint8_t *, size_t> requestCallback(uint8_t *buffer, size_t len);

public:
    ThingSetServer(ThingSetServerTransport &transport);

    bool listen();
};

} // namespace ThingSet