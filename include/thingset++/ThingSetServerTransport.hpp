/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <cstdint>
#include <cstdio>
#include <functional>
#include "thingset++/StreamingThingSetBinaryEncoder.hpp"

namespace ThingSet {

/// @brief Interface for transports for ThingSet servers.
/// @tparam Identifier Type of client identifier
/// @tparam Size Size of broadcast message frames
/// @tparam Encoder Type of streaming encoder
template <typename Identifier, size_t Size, StreamingBinaryEncoder<Size> Encoder>
class ThingSetServerTransport
{
public:
    /// @brief Core listening method. The supplied callback is invoked when a request
    /// is received.
    /// @param callback The callback to be invoked when a request is received.
    /// @return True.
    virtual bool listen(std::function<int(const Identifier &, uint8_t *, size_t, uint8_t *, size_t)> callback) = 0;

    virtual Encoder getPublishingEncoder() = 0;
};

} // namespace ThingSet