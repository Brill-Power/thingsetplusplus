/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "thingset++/zsock/ThingSetZephyrSocketClientTransport.hpp"
#include "thingset++/ThingSetStatus.hpp"
#include <assert.h>
#include <zephyr/kernel.h>
#include <zephyr/net/net_if.h>
#include <zephyr/net/net_ip.h>
#include <zephyr/net/socket.h>

#define UDP_THREAD_STACK_SIZE 1024
#define UDP_THREAD_PRIORITY   2

K_THREAD_STACK_DEFINE(udp_thread_stack, UDP_THREAD_STACK_SIZE);
static struct k_thread udp_thread;

namespace ThingSet::Zsock {

ThingSetZephyrSocketClientTransport::ThingSetZephyrSocketClientTransport(struct net_if *iface, const char *ip,
                                                                         uint16_t port)
    : _requestResponseSocket(-1), _subscribeSocket(-1), _listenerThreadID(-1)
{
    // TCP address configuration - uses user-supplied IP address and port
    int ret = net_addr_pton(AF_INET, ip, &_nodeaddr.sin_addr);
    _nodeaddr.sin_family = AF_INET;
    _nodeaddr.sin_port = htons(port);

    // UDP address configuration - listens on user-supplied IP address, port 9002
    ret |= net_addr_pton(AF_INET, ip, &_udpaddr.sin_addr);
    _udpaddr.sin_family = AF_INET;
    _udpaddr.sin_port = htons(9002);

    // Add IP address to specified network interface
    struct net_if_addr *addr = net_if_ipv4_addr_add(iface, &_nodeaddr.sin_addr, NET_ADDR_MANUAL, 0);

    _requestResponseSocket = zsock_socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    _subscribeSocket = zsock_socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    __ASSERT(ret == 0, "Failed to convert %s to IP address: %d", ip, errno);
    __ASSERT(addr != NULL, "Failed to add IP address to interface: %d", errno);
    __ASSERT(_requestResponseSocket >= 0, "Failed to create TCP socket: %d", errno);
    __ASSERT(_subscribeSocket >= 0, "Failed to create UDP socket: %d", errno);
}

bool ThingSetZephyrSocketClientTransport::set_client_connection_params(const char *ip, uint16_t port)
{
    _clientaddr.sin_family = AF_INET;
    _clientaddr.sin_port = htons(port);

    return net_addr_pton(AF_INET, ip, &_clientaddr.sin_addr) == 0;
}

bool ThingSetZephyrSocketClientTransport::connect()
{
    int ret = zsock_connect(_requestResponseSocket, (struct sockaddr *)&_clientaddr, sizeof(_clientaddr));
    ret |= zsock_bind(_subscribeSocket, (struct sockaddr *)&_udpaddr, sizeof(_udpaddr));

    return ret == 0;
}

int ThingSetZephyrSocketClientTransport::read(uint8_t *buffer, size_t len)
{
    return zsock_recv(_requestResponseSocket, buffer, len, 0);
}

bool ThingSetZephyrSocketClientTransport::write(uint8_t *buffer, size_t len)
{
    return zsock_send(_requestResponseSocket, buffer, len, 0) == (ssize_t)len;
}

uint8_t *ThingSetZephyrSocketClientTransport::get_buffer(void)
{
    return _buffer;
}

int ThingSetZephyrSocketClientTransport::get_sub_sock(void)
{
    return _subscribeSocket;
}

std::function<void(uint8_t *, size_t)> ThingSetZephyrSocketClientTransport::get_callback(void)
{
    return _listenerCallback;
}

void udp_thread_loop(void *p1, void *p2, void *p3)
{
    // p1 is a pointer to an instance of ThingSetZephyrSocketClientTransport
    ThingSetZephyrSocketClientTransport *transport = static_cast<ThingSetZephyrSocketClientTransport *>(p1);

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
                transport->get_callback()(&buf[3], actual_length);
            }
        }
    }

    return;
}

void ThingSetZephyrSocketClientTransport::subscribe(std::function<void(uint8_t *, size_t)> callback)
{
    _listenerCallback = callback;
    _listenerThreadID = (int)k_thread_create(&udp_thread, udp_thread_stack, K_THREAD_STACK_SIZEOF(udp_thread_stack),
                                             udp_thread_loop, this, NULL, NULL, UDP_THREAD_PRIORITY, 0, K_NO_WAIT);
}

} // namespace ThingSet::Zsock