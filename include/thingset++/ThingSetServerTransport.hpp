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
template <size_t Size, typename T>
concept StreamingBinaryEncoder = std::is_base_of_v<StreamingThingSetBinaryEncoder<Size>, T>;

class _ThingSetServerTransport
{
    public:
    /// @brief Core listening method. The supplied callback is invoked when a request
    /// is received.
    /// @param callback The callback to be invoked when a request is received.
    /// @return True.
    virtual bool listen(std::function<int(uint8_t *, size_t, uint8_t *, size_t)> callback) = 0;

    /// @brief Publish a request using a broadcast mechanism appropriate to the underlying
    /// transport.
    /// @param buffer A pointer to the buffer to be broadcasted.
    /// @param len The length of the data in the buffer.
    /// @return True.
    virtual bool publish(uint8_t *buffer, size_t len) = 0;
};

template <size_t StreamingFrameSize, typename StreamingEncoder>
    requires StreamingBinaryEncoder<StreamingFrameSize, StreamingEncoder>
class ThingSetServerTransport : public _ThingSetServerTransport
{
public:
    virtual StreamingEncoder getPublishingEncoder() = 0;
};

} // namespace ThingSet