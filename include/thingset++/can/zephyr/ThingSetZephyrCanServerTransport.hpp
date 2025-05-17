/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "thingset++/can/zephyr/ThingSetZephyrCanInterface.hpp"
#include "thingset++/can/ThingSetCanServerTransport.hpp"
extern "C" {
#include <canbus/isotp_fast.h>
}

namespace ThingSet::Can::Zephyr {

class ThingSetZephyrCanServerTransport : public Can::ThingSetCanServerTransport
{
private:
    _ThingSetZephyrCanInterface &_canInterface;
    isotp_fast_ctx _requestResponseContext;
    k_sem _lock;
    uint8_t *_rxBuffer;
    size_t _rxBufferSize;
    uint8_t *_txBuffer;
    size_t _txBufferSize;
    std::function<int(const CanID &, uint8_t *, size_t, uint8_t *, size_t)> _inboundRequestCallback;

public:
    ThingSetZephyrCanServerTransport(_ThingSetZephyrCanInterface &canInterface, uint8_t *rxBuffer,
                                     size_t rxBufferSize, uint8_t *txBuffer, size_t txBufferSize);
    ~ThingSetZephyrCanServerTransport();

    bool listen(std::function<int(const CanID &, uint8_t *, size_t, uint8_t *, size_t)> callback) override;

protected:
    ThingSetCanInterface &getInterface() override;

    bool doPublish(const Can::CanID &id, uint8_t *buffer, size_t length) override;

private:
    static void onRequestResponseReceived(net_buf *buffer, int remainingLength, isotp_fast_addr address, void *arg);
};

} // namespace ThingSet::Can::Zephyr