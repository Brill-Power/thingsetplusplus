/*
 * Copyright (c) 2024 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */
#pragma once

#include <cstdint>
#include <cstdio>
#include <functional>

namespace ThingSet {

class ThingSetServerTransport
{
public:
    virtual bool listen(std::function<std::pair<uint8_t *, size_t>(uint8_t *, size_t)> callback) = 0;

    virtual bool publish(uint8_t *buffer, size_t len) = 0;
};

} // namespace ThingSet