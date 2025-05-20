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

void subscribe_thread_loop(void *p1, void *p2, void *p3);

ThingSetZephyrSocketSubscriptionTransport::ThingSetZephyrSocketSubscriptionTransport(struct net_if *iface) : _sub_sock(-1)
{
    net_addr_pton(AF_INET, "0.0.0.0", &_udp_addr.sin_addr);

    _udp_addr.sin_family = AF_INET;
    _udp_addr.sin_port = htons(9003);

    struct net_if_addr *addr = net_if_ipv4_addr_add(iface, &_udp_addr.sin_addr, NET_ADDR_MANUAL, 0);
    __ASSERT(addr != NULL, "Failed to add any address to interface: %d", errno);

    _sub_sock = zsock_socket(_udp_addr.sin_family, SOCK_DGRAM, IPPROTO_UDP);
    __ASSERT(_sub_sock >= 0, "Failed to create UDP socket: %d", errno);
}

ThingSetZephyrSocketSubscriptionTransport::~ThingSetZephyrSocketSubscriptionTransport()
{
    zsock_close(_sub_sock);
    _sub_sock = -1;
}

bool ThingSetZephyrSocketSubscriptionTransport::subscribe(std::function<void(const DummyEndpoint &, ThingSetBinaryDecoder &)> callback)
{
    int ret = zsock_bind(_sub_sock, (struct sockaddr *)&_udp_addr, sizeof(_udp_addr));

    if (ret) {
        return false;
    }

    _listener_callback = callback;
    _listener_tid =
        (int)k_thread_create(&subscribe_thread, subscribe_thread_stack, K_THREAD_STACK_SIZEOF(subscribe_thread_stack),
                             subscribe_thread_loop, this, NULL, NULL, SUBSCRIBE_THREAD_PRIORITY, 0, K_NO_WAIT);

    return true;
}

uint8_t *ThingSetZephyrSocketSubscriptionTransport::get_buffer(void)
{
    return _buffer;
}

std::function<void(const DummyEndpoint &, ThingSetBinaryDecoder &)> ThingSetZephyrSocketSubscriptionTransport::get_callback(void)
{
    return _listener_callback;
}

int ThingSetZephyrSocketSubscriptionTransport::get_sub_sock(void)
{
    return _sub_sock;
}

void subscribe_thread_loop(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    ThingSetZephyrSocketSubscriptionTransport *transport = static_cast<ThingSetZephyrSocketSubscriptionTransport *>(p1);

    for (;;) {
        uint8_t *buf = transport->get_buffer();

        size_t length = zsock_recv(transport->get_sub_sock(), buf, 1024, 0);

        printk("length: %d\n", length);

        if (length > 0) {
            for (size_t i = 0; i < length; i++) {
                printk("%02X ", buf[i]);
            }

            printk("\n");
        }

        if (length > 4) {
            if (buf[0] != ThingSetRequestType::report) {
                continue;
            }

            size_t actual_length = buf[1] | (buf[2] << 8);

            if (actual_length == (length - 3)) {
                DummyEndpoint E;
                DefaultFixedDepthThingSetBinaryDecoder decoder(&buf[3], actual_length, 2);
                transport->get_callback()(E, decoder);
            }
        }
    }
}

} // namespace ThingSet::Ip::Zsock