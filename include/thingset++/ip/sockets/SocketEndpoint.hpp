/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#ifdef __ZEPHYR__
#include <zephyr/net/net_if.h>
#include <zephyr/net/socket.h>
#else
#include <arpa/inet.h>
#endif // __ZEPHYR__
#include <ostream>

namespace ThingSet::Ip::Sockets {

class SocketEndpoint : public sockaddr_in {
};

std::ostream &operator<<(std::ostream &os, SocketEndpoint &ep);

} // namespace ThingSet::Ip::Sockets