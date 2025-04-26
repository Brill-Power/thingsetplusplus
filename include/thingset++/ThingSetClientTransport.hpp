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
    /// @brief Reads data from the transport into the supplied buffer.
    /// @param buffer A pointer to the buffer into which the data should be read.
    /// @param len The length of the buffer.
    /// @return The number of bytes read.
    virtual int read(uint8_t *buffer, size_t len) = 0;
    /// @brief Write the request in the supplied buffer to the transport.
    /// @param buffer A pointer to the buffer containing the request.
    /// @param len The length of the request in the buffer.
    /// @return True if the request was successfully written.
    virtual bool write(uint8_t *buffer, size_t len) = 0;
    /// @brief Subscribes for publications delivered via the transport's
    /// broadcast mechanism.
    /// @param callback A callback that is invoked when a published message
    /// is received.
    virtual void subscribe(std::function<void(uint8_t *, size_t)> callback) = 0;
};

} // namespace ThingSet