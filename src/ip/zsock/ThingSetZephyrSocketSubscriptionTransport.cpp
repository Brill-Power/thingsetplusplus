/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "thingset++/ip/zsock/ThingSetZephyrSocketSubscriptionTransport.hpp"
#include "thingset++/ThingSetStatus.hpp"
#include <assert.h>
#include <zephyr/kernel.h>

#define SUBSCRIBE_THREAD_STACK_SIZE 1024
#define SUBSCRIBE_THREAD_PRIORITY   2

K_THREAD_STACK_DEFINE(subscribe_thread_stack, SUBSCRIBE_THREAD_STACK_SIZE);
static struct k_thread subscribe_thread;

namespace ThingSet::Ip::Zsock {

ThingSetZephyrSocketSubscriptionTransport::ThingSetZephyrSocketSubscriptionTransport(net_if *iface) : _listenSocketHandle(-1)
{
    _listenAddress.sin_addr.s_addr = 0x0;
    _listenAddress.sin_family = AF_INET;
    _listenAddress.sin_port = htons(9002);

    _listenSocketHandle = zsock_socket(_listenAddress.sin_family, SOCK_DGRAM, IPPROTO_UDP);
    __ASSERT(_listenSocketHandle >= 0, "Failed to create UDP socket: %d", errno);
}

ThingSetZephyrSocketSubscriptionTransport::~ThingSetZephyrSocketSubscriptionTransport()
{
    zsock_close(_listenSocketHandle);
    _listenSocketHandle = -1;
}

bool ThingSetZephyrSocketSubscriptionTransport::subscribe(std::function<void(const SocketEndpoint &, ThingSetBinaryDecoder &)> callback)
{
    int ret = zsock_bind(_listenSocketHandle, (sockaddr *)&_listenAddress, sizeof(_listenAddress));

    if (ret) {
        return false;
    }

    _callback = callback;
    _listenerThreadId =
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
        size_t length = zsock_recvfrom(_listenSocketHandle, _buffer, 1024, 0, (sockaddr *)&sourceAddress, &sourceAddressSize);

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