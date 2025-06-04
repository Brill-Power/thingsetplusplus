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
#include <netinet/in.h>
#include <ostream>
#endif // __ZEPHYR__

namespace ThingSet::Ip::Sockets {

class SocketEndpoint : public sockaddr_in {
};

#ifndef __ZEPHYR__
std::ostream &operator<<(std::ostream &os, SocketEndpoint &ep);
#endif // __ZEPHYR__

} // namespace ThingSet::Ip::Sockets
