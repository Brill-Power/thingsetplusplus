/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <zephyr/net/net_if.h>
#include <zephyr/net/socket.h>
#include <ostream>

namespace ThingSet::Ip::Zsock {

class SocketEndpoint : public sockaddr_in {
};

std::ostream &operator<<(std::ostream &os, SocketEndpoint &ep);

} // namespace ThingSet::Ip::Zsock