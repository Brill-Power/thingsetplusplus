/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <cstdint>
#include <cstdio>
#include <functional>

namespace ThingSet {

/// @brief Interface for client transports.
class ThingSetClientTransport
{
public:
    virtual bool connect() = 0;
    virtual int read(uint8_t *buffer, size_t len) = 0;
    virtual bool write(uint8_t *buffer, size_t len) = 0;
    virtual void subscribe(std::function<void(uint8_t *, size_t)> callback) = 0;
};

} // namespace ThingSet