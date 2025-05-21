/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "thingset++/ip/zsock/ThingSetZephyrSocketSubscriptionTransport.hpp"
#include "thingset++/ThingSetStatus.hpp"
#include <assert.h>
#include <zephyr/kernel.h>
#include <zephyr/net/net_if.h>
#include <zephyr/net/net_ip.h>
#include <zephyr/net/socket.h>

#define SUBSCRIBE_THREAD_STACK_SIZE 1024
#define SUBSCRIBE_THREAD_PRIORITY   2

K_THREAD_STACK_DEFINE(subscribe_thread_stack, SUBSCRIBE_THREAD_STACK_SIZE);
static struct k_thread subscribe_thread;

namespace ThingSet::Ip::Zsock {

ThingSetZephyrSocketSubscriptionTransport::ThingSetZephyrSocketSubscriptionTransport(net_if *iface, const char *ip) : _sub_sock(-1)
{
    sockaddr_in _subnet_addr = {
        .sin_family = AF_INET,
    };
    net_addr_pton(AF_INET, ip, &_subnet_addr.sin_addr);

    net_if_addr *addr = net_if_ipv4_addr_add(iface, &_subnet_addr.sin_addr, NET_ADDR_MANUAL, 0);
    __ASSERT(addr != NULL, "Failed to add address to interface: %d", errno);

    net_addr_pton(AF_INET, "0.0.0.0", &_udp_addr.sin_addr);
    _udp_addr.sin_family = AF_INET;
    _udp_addr.sin_port = htons(9002);

    _sub_sock = zsock_socket(_udp_addr.sin_family, SOCK_DGRAM, IPPROTO_UDP);
    __ASSERT(_sub_sock >= 0, "Failed to create UDP socket: %d", errno);

    sockaddr_in mask;
    net_addr_pton(AF_INET, "255.255.255.0", &mask.sin_addr);
    net_if_ipv4_set_netmask_by_addr(iface, &_udp_addr.sin_addr, &mask.sin_addr);
}

ThingSetZephyrSocketSubscriptionTransport::~ThingSetZephyrSocketSubscriptionTransport()
{
    zsock_close(_sub_sock);
    _sub_sock = -1;
}

bool ThingSetZephyrSocketSubscriptionTransport::subscribe(std::function<void(const SocketEndpoint &, ThingSetBinaryDecoder &)> callback)
{
    int ret = zsock_bind(_sub_sock, (sockaddr *)&_udp_addr, sizeof(_udp_addr));

    if (ret) {
        return false;
    }

    _callback = callback;
    _listener_tid =
        k_thread_create(&subscribe_thread, subscribe_thread_stack, K_THREAD_STACK_SIZEOF(subscribe_thread_stack),
                        runSubscriber, this, NULL, NULL, SUBSCRIBE_THREAD_PRIORITY, 0, K_NO_WAIT);

    return true;
}

void ThingSetZephyrSocketSubscriptionTransport::runSubscriber(void *p1, void *, void *)
{
    ThingSetZephyrSocketSubscriptionTransport *transport = static_cast<ThingSetZephyrSocketSubscriptionTransport *>(p1);
    transport->runSubscriber();
}

void ThingSetZephyrSocketSubscriptionTransport::runSubscriber()
{
    for (;;) {
        SocketEndpoint sourceAddress;
        socklen_t sourceAddressSize = sizeof(sourceAddress);
        size_t length = zsock_recvfrom(_sub_sock, _buffer, 1024, 0, (sockaddr *)&sourceAddress, &sourceAddressSize);
        printk("length: %d\n", length);

        if (length > 4) {
            if (_buffer[0] != ThingSetRequestType::report) {
                continue;
            }

            size_t actual_length = _buffer[1] | (_buffer[2] << 8);

            if (actual_length == (length - 3)) {
                DefaultFixedDepthThingSetBinaryDecoder decoder(&_buffer[3], actual_length, 2);
                _callback(sourceAddress, decoder);
            }
        }
    }
}

} // namespace ThingSet::Ip::Zsock