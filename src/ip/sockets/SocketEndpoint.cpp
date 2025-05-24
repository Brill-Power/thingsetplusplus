/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "thingset++/ip/sockets/SocketEndpoint.hpp"
#include "thingset++/ip/sockets/ZephyrSocketStubs.h"

namespace ThingSet::Ip::Sockets {

std::ostream &operator<<(std::ostream &os, SocketEndpoint &ep)
{
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &ep.sin_addr, ip, sizeof(ip));
    return os << ip << ":" << std::dec << ep.sin_port;
}

} // namespace ThingSet::Ip::Sockets