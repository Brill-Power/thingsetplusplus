/*
 * Copyright (c) 2024 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */
#pragma once

#include "can/ThingSetCanServerTransport.hpp"
#include "can/socketcan/ThingSetSocketCanInterface.hpp"

namespace ThingSet::Can::SocketCan {

class ThingSetSocketCanServerTransport : public Can::ThingSetCanServerTransport
{
private:
    ThingSetSocketCanInterface _canInterface;

public:
    ThingSetSocketCanServerTransport(const std::string &deviceName);

    bool listen(std::function<int(uint8_t *, size_t, uint8_t *, size_t)> callback) override;
    bool publish(Can::CanID &id, uint8_t *buffer, size_t length) override;
};

} // namespace ThingSet::Can::SocketCan