/*
 * Copyright (c) 2024 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */

#include "Eui.hpp"
#include <bit>
#include <string.h>

#ifdef __linux__

#include <ifaddrs.h>
#include <linux/if_packet.h>
#include <net/if_arp.h>

#elif defined(__ZEPHYR__)

#include <zephyr/drivers/hwinfo.h>
#include <zephyr/sys/crc.h>

#endif

namespace ThingSet {

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

#ifdef __linux__

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

Eui::Eui()
{
    getEthernetMacAddress(_value, _array);
}

#elif defined(__ZEPHYR__)

static bool getHardwareIdentifier(std::array<uint8_t, 8> &array)
{
    uint8_t buf[12] = { 0 };
    uint32_t crc;
    hwinfo_get_device_id(buf, sizeof(buf));

    crc = crc32_ieee(buf, 8);
    memcpy(array.data(), &crc, 4);
    crc = crc32_ieee(buf + 4, 8);
    memcpy(array.data() + 4, &crc, 4);

    /* set U/L bit to 0 for locally administered (not globally unique) EUIs */
    array[0] &= ~(1U << 1);
    return true;
}

Eui::Eui()
{
    getHardwareIdentifier(_array);
    _value = 0;
    for (size_t i = 0; i < _array.size(); i++) {
        _value |= (uint64_t)_array[i] << (8 * (_array.size() - (i + 1)));
    }
}

#endif

} // namespace ThingSet