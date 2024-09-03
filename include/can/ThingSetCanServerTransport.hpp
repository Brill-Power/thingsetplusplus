/*
 * Copyright (c) 2024 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */
#pragma once

#include "ThingSetServerTransport.hpp"
#include "can/CanID.hpp"

namespace ThingSet::Can {

class ThingSetCanServerTransport : public ThingSetServerTransport
{
protected:
    uint8_t _messageNumber;

    ThingSetCanServerTransport();

public:
    bool publish(uint8_t *buffer, size_t len) override;
    virtual bool publish(Can::CanID &id, uint8_t *buffer, size_t length) = 0;
};

} // namespace ThingSet::Can