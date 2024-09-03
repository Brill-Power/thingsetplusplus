/*
 * Copyright (c) 2024 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */

#include "Eui.hpp"
#include <ifaddrs.h>
#include <linux/if_packet.h>
#include <net/if_arp.h>
#include <string.h>

/// @brief Gets the MAC address from the first Ethernet address found on the system.
/// @param t A reference to an array that will contain the MAC address.
/// @return True if an Ethernet interface was found.
static bool getEthernetMacAddress(std::array<uint8_t, 8> &t)
{
    ifaddrs *ptr_ifaddrs = nullptr;

    auto result = getifaddrs(&ptr_ifaddrs);
    if (result != 0) {
        // failed
        return false;
    }

    for (ifaddrs *ptr_entry = ptr_ifaddrs; ptr_entry != nullptr; ptr_entry = ptr_entry->ifa_next) {
        if (ptr_entry->ifa_addr && ptr_entry->ifa_addr->sa_family == AF_PACKET) {
            sockaddr_ll *s = (sockaddr_ll *)ptr_entry->ifa_addr;
            if (s->sll_hatype == ARPHRD_ETHER && s->sll_halen < t.size()) {
                memcpy(t.data(), s->sll_addr, s->sll_halen);
                return true;
            }
        }
    }

    return false;
}

namespace ThingSet {
Eui::Eui()
{
    getEthernetMacAddress(value);
}

Eui &Eui::getInstance()
{
    static Eui instance;
    return instance;
}

std::array<uint8_t, 8> Eui::getValue()
{
    return getInstance().value;
}
} // namespace ThingSet
