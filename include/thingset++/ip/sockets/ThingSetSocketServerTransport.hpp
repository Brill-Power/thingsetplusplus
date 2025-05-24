/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "thingset++/ip/ThingSetIpServerTransport.hpp"
#include "thingset++/ip/sockets/SocketEndpoint.hpp"
#include <cstdint>
#include <cstdio>
#ifdef __ZEPHYR__
#include <zephyr/kernel.h>
#include <zephyr/net/net_if.h>
#include <zephyr/net/socket.h>
#else
#include <thread>
#include <sys/socket.h>
#include <arpa/inet.h>
#endif // __ZEPHYR__

namespace ThingSet::Ip::Sockets {

class ThingSetSocketServerTransport : public ThingSetIpServerTransport<SocketEndpoint>
{
private:
    sockaddr_in _publishAddress;
    sockaddr_in _listenAddress;
    sockaddr_in _broadcastAddress;
    int _publishSocketHandle;
    int _listenSocketHandle;
#ifdef __ZEPHYR__
    k_tid_t _acceptorThreadId;
    k_tid_t _handlerThreadId;
#else
    std::thread _acceptorThread;
    std::thread _handlerThread;
#endif // __ZEPHYR__
    std::function<int(const SocketEndpoint &, uint8_t *, size_t, uint8_t *, size_t)> _callback;

private:
    ThingSetSocketServerTransport(const std::pair<in_addr, in_addr> &ipAddressAndSubnet);

public:
#ifdef __ZEPHYR__
    ThingSetSocketServerTransport(net_if *iface);
#else
    ThingSetSocketServerTransport();
    ThingSetSocketServerTransport(const std::string &interface);
#endif // __ZEPHYR__
    ~ThingSetSocketServerTransport();

    bool listen(std::function<int(const SocketEndpoint &, uint8_t *, size_t, uint8_t *, size_t)> callback) override;
    bool publish(uint8_t *buffer, size_t len) override;

private:
    // TODO: consider https://stackoverflow.com/questions/51451843/creating-a-template-to-wrap-c-member-functions-and-expose-as-c-callbacks

#ifdef __ZEPHYR__
    static void runAcceptor(void *p1, void *, void *);
    static void runHandler(void *p1, void *, void *);
#endif // __ZEPHYR__
    void runAcceptor();
    void runHandler();
};

} // namespace ThingSet::Ip::Sockets
