/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "thingset++/can/ThingSetCanInterface.hpp"
#include <functional>
#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/can.h>

namespace ThingSet::Can::Zephyr {

class _ThingSetZephyrCanInterface : public ThingSetCanInterface
{
protected:
    const device *const _canDevice;
    k_sem _lock;

    _ThingSetZephyrCanInterface(const device *const canDevice);

public:
    const device *const getDevice();
};

class ThingSetZephyrCanStubInterface : public _ThingSetZephyrCanInterface
{
public:
    ThingSetZephyrCanStubInterface(const device *const canDevice);

    using ThingSetCanInterface::bind;
    bool bind(uint8_t nodeAddress) override;
};

/// @brief Encapsulates a ThingSet CAN interface.
class ThingSetZephyrCanInterface : public _ThingSetZephyrCanInterface
{
private:
    enum AddressClaimEvent
    {
        alreadyUsed = 0x01,
        discoverMessageSent = 0x02,
    };

    k_event _events;
    int _claimFilterId;
    int _discoverFilterId;

public:
    ThingSetZephyrCanInterface(ThingSetZephyrCanInterface &&) = delete;
    ThingSetZephyrCanInterface(const ThingSetZephyrCanInterface &) = delete;
    ThingSetZephyrCanInterface(const device *const canDevice);
    ~ThingSetZephyrCanInterface();

    using ThingSetCanInterface::bind;
    bool bind(uint8_t nodeAddress) override;

private:
    static void onAddressDiscoverReceived(const device *dev, can_frame *frame, void *arg);
    static void onAnyAddressDiscoverReceived(const device *dev, can_frame *frame, void *arg);
    static void onAddressClaimReceived(const device *dev, can_frame *frame, void *arg);
    static void onAddressDiscoverSent(const device *dev, int error, void *arg);
    int addFilter(CanID &canId, void (*callback)(const device *, can_frame *, void *));
    bool claimAddress(uint8_t nodeAddress);
};

} // namespace ThingSet::Can::Zephyr