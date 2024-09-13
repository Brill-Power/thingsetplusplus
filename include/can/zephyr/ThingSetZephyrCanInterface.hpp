/*
 * Copyright (c) 2024 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */
#pragma once

#include "can/ThingSetCanInterface.hpp"
#include <functional>
#include <zephyr/device.h>
#include "isotp_fast.hpp"

namespace ThingSet::Can::Zephyr {

/// @brief Encapsulates a ThingSet CAN interface.
class ThingSetZephyrCanInterface : public ThingSetCanInterface
{
private:
    struct Listener {
        isotp_fast_ctx requestResponseContext;
        std::function<int(uint8_t *, size_t, uint8_t *, size_t)> callback;
    };

    struct AddressDiscoverer {
        uint8_t nodeAddress;
        k_event events;

        static void onAddressClaimReceived(const struct device *dev, struct can_frame *frame, void *arg);
        static void onAddressDiscoverSent(const struct device *dev, int error, void *arg);
    };

    enum AddressClaimEvent {
        alreadyUsed = 0x01,
        discoverMessageSent = 0x02,
    };

    const device *const _canDevice;
    Listener _listener;

public:
    ThingSetZephyrCanInterface(const device *const canDevice);
    ~ThingSetZephyrCanInterface();

    using ThingSetCanInterface::bind;
    bool bind(uint8_t nodeAddress) override;

    bool listen(std::function<int(uint8_t *, size_t, uint8_t *, size_t)> callback) override;

    bool publish(CanID &id, uint8_t *buffer, size_t length) override;

private:
    static void onRequestResponseReceived(net_buf *buffer, int remainingLength, isotp_fast_addr address, void *arg);
    bool tryClaimAddress(uint8_t nodeAddress);
};

} // namespace ThingSet::Can::Zephyr