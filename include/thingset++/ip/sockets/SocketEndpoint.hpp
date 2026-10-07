/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#ifdef __ZEPHYR__
/* Zephyr's C headers declare functions named after the structs they take (fs_statvfs,
 * net_if_mcast_monitor). In C++ that hides the struct's implicit constructor, which
 * -Wshadow reports in every file that includes this header. Nothing here uses those
 * constructors, so the check is suspended for these includes only. */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#include <zephyr/net/net_if.h>
#include <zephyr/net/socket.h>
#pragma GCC diagnostic pop
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
