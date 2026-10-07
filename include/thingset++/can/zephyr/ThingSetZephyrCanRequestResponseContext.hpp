/*
 * Copyright (c) 2025 Brill Power.
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

#define THINGSET_PLUS_PLUS_ZEPHYR_CAN_FILTER_ID_NONE -1

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
    template <size_t RxSize, size_t TxSize>
    ThingSetZephyrCanRequestResponseContext(ThingSetZephyrCanInterface &zephyrCanInterface,
                                            std::array<uint8_t, RxSize> &rxStorage,
                                            std::array<uint8_t, TxSize> &txStorage)
        : _canInterface(zephyrCanInterface), _rxBuffer(rxStorage.data()), _rxBufferSize(RxSize),
          _txBuffer(txStorage.data()), _txBufferSize(TxSize)
    {
        _requestResponseContext.filter_id = THINGSET_PLUS_PLUS_ZEPHYR_CAN_FILTER_ID_NONE;
        k_sem_init(&_lock, 1, 1);
    }
    ~ThingSetZephyrCanRequestResponseContext();

    ThingSetZephyrCanInterface &getInterface();

    bool bind(std::function<int(const CanID &, uint8_t *, size_t, uint8_t *, size_t)> callback);
    bool bind(uint8_t otherNodeAddress, std::function<int(const CanID &, uint8_t *, size_t, uint8_t *, size_t)> callback);

    bool send(const uint8_t otherNodeAddress, uint8_t *buffer, size_t len);

private:
    void unbindIfNecessary();
    static void onRequestResponseReceived(net_buf *buffer, int remainingLength, isotp_fast_addr address, void *arg);
    void onRequestResponseReceived(net_buf *buffer, int remainingLength, isotp_fast_addr address);
    static const isotp_fast_opts flowControlOptions;
    static const isotp_fast_opts peerFlowControlOptions;
};

} // namespace ThingSet::Can::Zephyr