/*
 * Copyright (c) 2024 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */

#ifdef __linux__

#include "Eui.hpp"
#include <bit>
#include <ifaddrs.h>
#include <linux/if_packet.h>
#include <net/if_arp.h>
#include <string.h>

/// @brief Gets the MAC address from the first Ethernet address found on the system.
/// @param t A reference to an array that will contain the MAC address.
/// @return True if an Ethernet interface was found.
static bool getEthernetMacAddress(uint64_t &target, std::array<uint8_t, 8> &array)
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
            if (s->sll_hatype == ARPHRD_ETHER && s->sll_halen < sizeof(target)) {
                if constexpr (std::endian::native == std::endian::big) {
                    memcpy(&target, s->sll_addr, s->sll_halen);
                }
                else {
                    target = 0;
                    for (size_t i = 0; i < s->sll_halen; i++) {
                        target |= (uint64_t)s->sll_addr[i] << (8 * (s->sll_halen - (i + 1)));
                    }
                }
                memcpy(&array.data()[array.size() - s->sll_halen], s->sll_addr, s->sll_halen);
                return true;
            }
        }
    }

    return false;
}

namespace ThingSet {
Eui::Eui()
{
    getEthernetMacAddress(_value, _array);
}

Eui &Eui::getInstance()
{
    static Eui instance;
    return instance;
}

const uint64_t &Eui::getValue()
{
    return getInstance()._value;
}

const std::array<uint8_t, 8> &Eui::getArray()
{
    return getInstance()._array;
}
} // namespace ThingSet

#endif