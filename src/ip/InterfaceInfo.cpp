/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "thingset++/ip/InterfaceInfo.hpp"
#include <net/if.h>
#include <netinet/in.h>
#include <ifaddrs.h>

namespace ThingSet::Ip {

bool InterfaceInfo::get(const std::string &interfaceName, in_addr &address, in_addr &subnet)
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
                address = socketAddressInterface->sin_addr;
                socketAddressInterface = reinterpret_cast<sockaddr_in *>(interface->ifa_netmask);
                subnet = socketAddressInterface->sin_addr;
                freeifaddrs(interfaces);
                return true;
            }
        }
    }

    freeifaddrs(interfaces);
    return false;
}

}
