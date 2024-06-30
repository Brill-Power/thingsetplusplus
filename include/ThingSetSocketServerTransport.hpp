/*
 * Copyright (c) 2024 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */
#pragma once

#include "ThingSetServerTransport.hpp"
#include <functional>
#include <netinet/in.h>

namespace ThingSet {

class ThingSetSocketServerTransport : ThingSetServerTransport
{
private:
    sockaddr_in _address;
    int _socket;

public:
    ThingSetSocketServerTransport();

    bool listen(std::function<std::pair<uint8_t *, size_t>(uint8_t *, size_t)> callback) override;
};

} // namespace ThingSet