/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "thingset++/ThingSetServerTransport.hpp"
#include "thingset++/can/CanID.hpp"
#include "thingset++/can/ThingSetCanInterface.hpp"

namespace ThingSet::Can {

class ThingSetCanServerTransport : public ThingSetServerTransport<CanID>
{
protected:
    uint8_t _messageNumber;

    ThingSetCanServerTransport();

    virtual ThingSetCanInterface &getInterface() = 0;

public:
    uint8_t getNodeAddress();

    bool publish(uint8_t *buffer, size_t len) override;
    bool publish(Can::CanID &id, uint8_t *buffer, size_t length);

    bool listen(std::function<int(CanID &, uint8_t *, size_t, uint8_t *, size_t)> callback) override;
};

} // namespace ThingSet::Can