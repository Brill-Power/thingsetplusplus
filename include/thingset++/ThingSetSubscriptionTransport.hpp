/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <cstdint>
#include <functional>

namespace ThingSet {

/// @brief Interface for client subscription transports.
/// @tparam Identifier Type of client identifier
template <typename Identifier>
class ThingSetSubscriptionTransport
{
public:
    virtual bool connect() = 0;
    /// @brief Subscribes for publications delivered via the transport's
    /// broadcast mechanism.
    /// @param callback A callback that is invoked when a published message
    /// is received.
    virtual void subscribe(std::function<void(Identifier &, uint8_t *, size_t)> callback) = 0;
};

} // namespace ThingSet