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
extern "C" {
#include <canbus/isotp_fast.h>
}

namespace ThingSet::Can::Zephyr {

class _ThingSetZephyrCanInterface : public ThingSetCanInterface
{
protected:
    const device *const _canDevice;
    isotp_fast_ctx _requestResponseContext;
    k_sem _lock;
    uint8_t *_rxBuffer;
    size_t _rxBufferSize;
    uint8_t *_txBuffer;
    size_t _txBufferSize;
    std::function<int(const CanID &, uint8_t *, size_t, uint8_t *, size_t)> _inboundRequestCallback;

    _ThingSetZephyrCanInterface(const device *const canDevice, uint8_t *rxBuffer, size_t rxBufferSize,
                                uint8_t *txBuffer, size_t txBufferSize);
    ~_ThingSetZephyrCanInterface();

public:
    const device *const getDevice();

    static const isotp_fast_opts flowControlOptions;

    static void onRequestResponseReceived(net_buf *buffer, int remainingLength, isotp_fast_addr address, void *arg);

    bool bindRequestResponse(const uint8_t otherNodeAddress, std::function<int(const CanID &, uint8_t *, size_t, uint8_t *, size_t)> callback);
    bool bindRequestResponse(std::function<int(const CanID &, uint8_t *, size_t, uint8_t *, size_t)> callback);

    bool send(const uint8_t otherNodeAddress, uint8_t *buffer, size_t len);
};

class ThingSetZephyrCanStubInterface : public _ThingSetZephyrCanInterface
{
public:
    ThingSetZephyrCanStubInterface(const device *const canDevice, uint8_t *rxBuffer, size_t rxBufferSize,
                                   uint8_t *txBuffer, size_t txBufferSize);

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
    template <size_t RxSize, size_t TxSize>
    ThingSetZephyrCanInterface(const device *const canDevice, std::array<uint8_t, RxSize> &rxBuffer, std::array<uint8_t, TxSize> &txBuffer) : ThingSetZephyrCanInterface(canDevice, rxBuffer.data(), rxBuffer.size(), txBuffer.data(), txBuffer.size())
    {}

    ThingSetZephyrCanInterface(const device *const canDevice, uint8_t *rxBuffer, size_t rxBufferSize,
                               uint8_t *txBuffer, size_t txBufferSize);
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