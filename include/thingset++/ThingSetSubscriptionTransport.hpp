/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <cstdint>
#include <functional>
#include "thingset++/ThingSetBinaryDecoder.hpp"

namespace ThingSet {

/// @brief Interface for client subscription transports.
/// @tparam Identifier Type of client identifier
template <typename Identifier>
class ThingSetSubscriptionTransport
{
public:
    /// @brief Subscribes for publications delivered via the transport's
    /// broadcast mechanism.
    /// @param callback A callback that is invoked when a published message
    /// is received.
    virtual bool subscribe(std::function<void(const Identifier &, ThingSetBinaryDecoder &)> callback) = 0;
};

template <typename Identifier, typename T>
concept SubscriptionTransport = std::is_base_of_v<ThingSetSubscriptionTransport<Identifier>, T>;

} // namespace ThingSet