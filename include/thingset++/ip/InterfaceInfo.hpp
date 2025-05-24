/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <arpa/inet.h>
#include <string>

namespace ThingSet::Ip {

class InterfaceInfo {
public:
    static bool get(const std::string &interfaceName, in_addr &address, in_addr &subnet);
};

} // namespace ThingSet::Ip::Async