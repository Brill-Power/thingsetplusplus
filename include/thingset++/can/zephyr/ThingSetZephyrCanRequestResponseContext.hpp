/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <functional>
#include <thingset++/can/CanID.hpp>
#include <zephyr/kernel.h>
extern "C" {
#include <canbus/isotp_fast.h>
}

namespace ThingSet::Can::Zephyr {

class ThingSetZephyrCanInterface;

class ThingSetZephyrCanRequestResponseContext {
private:
    ThingSetZephyrCanInterface &_canInterface;
    isotp_fast_ctx _requestResponseContext;
    k_sem _lock;
    uint8_t *_rxBuffer;
    size_t _rxBufferSize;
    uint8_t *_txBuffer;
    size_t _txBufferSize;
    std::function<int(const CanID &, uint8_t *, size_t, uint8_t *, size_t)> _inboundRequestCallback;

public:
    template<size_t RxSize, size_t TxSize>
    ThingSetZephyrCanRequestResponseContext(ThingSetZephyrCanInterface &canInterface, std::array<uint8_t, RxSize> &rxBuffer,
        std::array<uint8_t, TxSize> &txBuffer) : _canInterface(canInterface), _rxBuffer(rxBuffer.data()), _rxBufferSize(RxSize),
        _txBuffer(txBuffer.data()), _txBufferSize(TxSize)
    {
        k_sem_init(&_lock, 1, 1);
    }
    ~ThingSetZephyrCanRequestResponseContext();

    ThingSetZephyrCanInterface &getInterface();

    bool bind(std::function<int(const CanID &, uint8_t *, size_t, uint8_t *, size_t)> callback);
    bool bind(uint8_t otherNodeAddress, std::function<int(const CanID &, uint8_t *, size_t, uint8_t *, size_t)> callback);

    bool send(const uint8_t otherNodeAddress, uint8_t *buffer, size_t len);

private:
    static void onRequestResponseReceived(net_buf *buffer, int remainingLength, isotp_fast_addr address, void *arg);
    void onRequestResponseReceived(net_buf *buffer, int remainingLength, isotp_fast_addr address);
    static const isotp_fast_opts flowControlOptions;
};

} // namespace ThingSet::Can::Zephyr