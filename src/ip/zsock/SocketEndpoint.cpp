/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "thingset++/ip/zsock/SocketEndpoint.hpp"

namespace ThingSet::Ip::Zsock {

std::ostream &operator<<(std::ostream &os, SocketEndpoint &ep)
{
    char ip[INET_ADDRSTRLEN];
    zsock_inet_ntop(AF_INET, &ep.sin_addr, ip, sizeof(ip));
    return os << ip << ":" << std::dec << ep.sin_port;
}

} // namespace ThingSet::Ip::Zsock