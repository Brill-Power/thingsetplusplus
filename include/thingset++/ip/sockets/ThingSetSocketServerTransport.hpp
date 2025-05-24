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

/// @brief Server transport using POSIX sockets.
class _ThingSetSocketServerTransport : public ThingSetIpServerTransport<SocketEndpoint>
{
private:
    sockaddr_in _publishAddress;
    sockaddr_in _listenAddress;
    sockaddr_in _broadcastAddress;
    int _publishSocketHandle;
    int _listenSocketHandle;
    std::function<int(const SocketEndpoint &, uint8_t *, size_t, uint8_t *, size_t)> _callback;

protected:
    _ThingSetSocketServerTransport(const std::pair<in_addr, in_addr> &ipAddressAndSubnet);

public:
    ~_ThingSetSocketServerTransport();

    bool listen(std::function<int(const SocketEndpoint &, uint8_t *, size_t, uint8_t *, size_t)> callback) override;
    bool publish(uint8_t *buffer, size_t len) override;

protected:
    virtual void startThreads() = 0;
    void runAcceptor();
    void runHandler();
};

class ThingSetSocketServerTransport : public _ThingSetSocketServerTransport
{
#ifdef __ZEPHYR__
private:
    k_tid_t _acceptorThreadId;
    k_tid_t _handlerThreadId;

public:
    ThingSetSocketServerTransport(net_if *iface);

protected:
    void startThreads() override;

private:
    // TODO: consider https://stackoverflow.com/questions/51451843/creating-a-template-to-wrap-c-member-functions-and-expose-as-c-callbacks
    using _ThingSetSocketServerTransport::runAcceptor;
    using _ThingSetSocketServerTransport::runHandler;
    static void runAcceptor(void *p1, void *, void *);
    static void runHandler(void *p1, void *, void *);
#else
private:
    std::thread _acceptorThread;
    std::thread _handlerThread;

public:
    ThingSetSocketServerTransport();
    ThingSetSocketServerTransport(const std::string &interface);

protected:
    void startThreads() override;
    #endif // __ZEPHYR__
};

} // namespace ThingSet::Ip::Sockets
