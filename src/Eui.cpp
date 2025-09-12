/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "thingset++/Eui.hpp"
#include <bit>
#include <string.h>

#if defined(__linux__) || defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__)
#include <net/if.h>
#include <ifaddrs.h>
#if defined(__linux__)
#include <linux/if_packet.h>
#elif defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__)
#include <net/if_dl.h>
#if defined(__FreeBSD__) || defined(__OpenBSD__)
#include <sys/socket.h>
#endif
#endif

// https://stackoverflow.com/questions/809902/64-bit-ntohl-in-c#4410728
#if defined(__linux__)
#include <endian.h>
#elif defined(__FreeBSD__) || defined(__NetBSD__)
#include <sys/endian.h>
#elif defined(__OpenBSD__)
#include <sys/types.h>
#define be64toh(x) betoh64(x)
#elif defined(__APPLE__)
#include <libkern/OSByteOrder.h>
#define be64toh(x) OSSwapBigToHostInt64(x)
#endif

#include <net/if_arp.h>

#elif defined(__ZEPHYR__)

#include <zephyr/drivers/hwinfo.h>
#include <zephyr/sys/crc.h>
#include <zephyr/net/net_ip.h>

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

const std::string Eui::getString()
{
    auto &arr = getInstance()._array;
    char str[17];
    snprintf(str, sizeof(str), "%02X%02X%02X%02X%02X%02X%02X%02X", arr[0], arr[1], arr[2], arr[3], arr[4], arr[5],
             arr[6], arr[7]);
    return std::string(str);
}

#if defined(__linux__) || defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__)

template <typename Source>
static void copyMacAddress(const Source &source, const size_t length, uint64_t &target, std::array<uint8_t, 8> &array)
{
    memcpy(&target, source, length);
    target = be64toh(target);
    memcpy(array.data(), source, length);
}

/// @brief Gets the MAC address from the first Ethernet interface found on the system.
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
        // skip loopback, etc.
        if ((ptr_entry->ifa_flags & IFF_LOOPBACK) ||
            ptr_entry->ifa_addr == NULL) {
            continue;
        }

#if defined(__linux__)
        if (ptr_entry->ifa_addr && ptr_entry->ifa_addr->sa_family == AF_PACKET) {
            sockaddr_ll *s = (sockaddr_ll *)ptr_entry->ifa_addr;
            if (s->sll_hatype == ARPHRD_ETHER && s->sll_halen == 6 && s->sll_halen < sizeof(target)) {
                copyMacAddress(s->sll_addr, s->sll_halen, target, array);
                freeifaddrs(ptr_ifaddrs);

                return true;
            }
#elif defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__)
        // https://stackoverflow.com/questions/6762766/mac-address-with-getifaddrs#26038501
        if (ptr_entry->ifa_addr && ptr_entry->ifa_addr->sa_family == AF_LINK) {
            sockaddr_dl *s = (sockaddr_dl *)ptr_entry->ifa_addr;
            if (s->sdl_alen == 6) {
                copyMacAddress(LLADDR(s), s->sdl_alen, target, array);
                freeifaddrs(ptr_ifaddrs);
                return true;
            }
#endif
        }
    }

    freeifaddrs(ptr_ifaddrs);
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
    memcpy(&_value, _array.data(), _array.size());
    if constexpr (std::endian::native != std::endian::big) {
        _value = ntohll(_value);
    }
}

#endif

} // namespace ThingSet
