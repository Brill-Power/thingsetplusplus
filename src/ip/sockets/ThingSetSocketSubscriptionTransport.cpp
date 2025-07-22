/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "thingset++/ip/sockets/ThingSetSocketSubscriptionTransport.hpp"
#include "thingset++/ip/StreamingUdpThingSetBinaryDecoder.hpp"
#include "thingset++/ip/sockets/ZephyrStubs.h"
#include "thingset++/ThingSetStatus.hpp"
#include <map>
#ifdef __ZEPHYR__
#include <assert.h>
#include <zephyr/kernel.h>

#define SUBSCRIBE_THREAD_STACK_SIZE 1024
#define SUBSCRIBE_THREAD_PRIORITY   2

K_THREAD_STACK_DEFINE(subscribe_thread_stack, SUBSCRIBE_THREAD_STACK_SIZE);
static struct k_thread subscribe_thread;
#else
#include <sys/socket.h>
#include <unistd.h>
#define __ASSERT(test, fmt, ...) { if (!(test)) { throw std::invalid_argument(fmt); } }
#endif // __ZEPHYR__

namespace ThingSet::Ip::Sockets {

_ThingSetSocketSubscriptionTransport::_ThingSetSocketSubscriptionTransport() : _listenSocketHandle(-1)
{
    _listenAddress.sin_addr.s_addr = 0x0;
    _listenAddress.sin_family = AF_INET;
    _listenAddress.sin_port = htons(9002);

    _listenSocketHandle = socket(_listenAddress.sin_family, SOCK_DGRAM, IPPROTO_UDP);
    __ASSERT(_listenSocketHandle >= 0, "Failed to create UDP socket: %d", errno);

    int optionValue = 1;
    int ret = setsockopt(_listenSocketHandle, SOL_SOCKET, SO_REUSEADDR, &optionValue, sizeof(optionValue));
    __ASSERT(ret == 0, "Failed to configure listen socket: %d", errno);
}

_ThingSetSocketSubscriptionTransport::~_ThingSetSocketSubscriptionTransport()
{
    close(_listenSocketHandle);
    _listenSocketHandle = -1;
}

bool _ThingSetSocketSubscriptionTransport::subscribe(std::function<void(const SocketEndpoint &, ThingSetBinaryDecoder &)> callback)
{
    int ret = bind(_listenSocketHandle, (sockaddr *)&_listenAddress, sizeof(_listenAddress));

    if (ret) {
        return false;
    }

    _callback = callback;
    startThread();

    return true;
}

void _ThingSetSocketSubscriptionTransport::runListener()
{
    std::map<in_addr_t, StreamingUdpThingSetBinaryDecoder>  decodersBySender;
    for (;;) {
        SocketEndpoint sourceAddress;
        socklen_t sourceAddressSize = sizeof(sourceAddress);
        Frame frame;
        frame.length = recvfrom(_listenSocketHandle, frame.buffer, THINGSET_STREAMING_MSG_SIZE, 0, (sockaddr *)&sourceAddress, &sourceAddressSize);
        SubscriptionListener::handle<MessageType>(frame, sourceAddress, sourceAddress.sin_addr.s_addr, decodersBySender, _callback);
    }
}

#ifdef __ZEPHYR__
void ThingSetSocketSubscriptionTransport::runListener(void *p1, void *, void *)
{
    ThingSetSocketSubscriptionTransport *transport = static_cast<ThingSetSocketSubscriptionTransport *>(p1);
    transport->runListener();
}

void ThingSetSocketSubscriptionTransport::startThread()
{
    _listenerThreadId =
        k_thread_create(&subscribe_thread, subscribe_thread_stack, K_THREAD_STACK_SIZEOF(subscribe_thread_stack),
                        runListener, this, NULL, NULL, SUBSCRIBE_THREAD_PRIORITY, 0, K_NO_WAIT);

}
#else
void ThingSetSocketSubscriptionTransport::startThread()
{
    _listenerThread = std::thread([&]()
    {
        runListener();
    });
}
#endif // __ZEPHYR__

} // namespace ThingSet::Ip::Sockets