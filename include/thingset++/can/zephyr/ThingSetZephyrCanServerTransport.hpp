/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "thingset++/can/zephyr/ThingSetZephyrCanInterface.hpp"
#include "thingset++/can/zephyr/ThingSetZephyrCanRequestResponseContext.hpp"
#include "thingset++/can/ThingSetCanServerTransport.hpp"

namespace ThingSet::Can::Zephyr {

class ThingSetZephyrCanServerTransport : public Can::ThingSetCanServerTransport
{
private:
    ThingSetZephyrCanRequestResponseContext _requestResponseContext;

public:
    ThingSetZephyrCanServerTransport(ThingSetZephyrCanServerTransport &&) = delete;
    ThingSetZephyrCanServerTransport(const ThingSetZephyrCanServerTransport &) = delete;

    template <size_t RxSize, size_t TxSize>
    ThingSetZephyrCanServerTransport(ThingSetZephyrCanInterface &canInterface, std::array<uint8_t, RxSize> &rxBuffer,
        std::array<uint8_t, TxSize> &txBuffer) : ThingSetCanServerTransport(),
        _requestResponseContext(canInterface, rxBuffer, txBuffer)
    {}
    ~ThingSetZephyrCanServerTransport();

    bool listen(std::function<int(const CanID &, uint8_t *, size_t, uint8_t *, size_t)> callback) override;

protected:
    ThingSetCanInterface &getInterface() override;

    bool doPublish(const Can::CanID &id, uint8_t *buffer, size_t length) override;
};

} // namespace ThingSet::Can::Zephyr