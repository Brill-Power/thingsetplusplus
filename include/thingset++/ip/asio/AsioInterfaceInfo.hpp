/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <asio/ip/address.hpp>
#include <string>

namespace ThingSet::Ip::Async {

class AsioInterfaceInfo {
public:
    static bool get(const std::string &interfaceName, asio::ip::address_v4 &address, asio::ip::address_v4 &broadcast);
};

} // namespace ThingSet::Ip::Async