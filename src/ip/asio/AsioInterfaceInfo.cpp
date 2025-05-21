/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "thingset++/ip/asio/AsioInterfaceInfo.hpp"
#include <sys/types.h>
#include <ifaddrs.h>
#include <iostream>

namespace ThingSet::Ip::Async {

bool AsioInterfaceInfo::get(const std::string &interfaceName, asio::ip::address_v4 &address, asio::ip::address_v4 &broadcast)
{
    ifaddrs *interfaces;
    if (getifaddrs(&interfaces)) {
        return false;
    }

    sockaddr_in *socketAddressInterface;
    for (ifaddrs *interface = interfaces; interface != nullptr; interface = interface->ifa_next) {
        if (interface->ifa_addr && interface->ifa_netmask && interface->ifa_addr->sa_family == AF_INET && (interface->ifa_flags & IFF_UP)) {
            if (interfaceName == interface->ifa_name) {
                socketAddressInterface = reinterpret_cast<sockaddr_in *>(interface->ifa_addr);
                address = asio::ip::make_address_v4(ntohl(socketAddressInterface->sin_addr.s_addr));
                socketAddressInterface = reinterpret_cast<sockaddr_in *>(interface->ifa_netmask);
                auto netmask = asio::ip::make_address_v4(ntohl(socketAddressInterface->sin_addr.s_addr));
                broadcast = asio::ip::make_address_v4(address.to_uint() | ( ~ netmask.to_uint()));
                freeifaddrs(interfaces);
                return true;
            }
        }
    }

    freeifaddrs(interfaces);
    return false;
}

}