/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "thingset++/ip/ThingSetIpServerTransport.hpp"
#include "thingset++/ip/zsock/SocketEndpoint.hpp"
#include <cstdint>
#include <cstdio>
#include <zephyr/kernel.h>
#include <zephyr/net/net_if.h>
#include <zephyr/net/socket.h>

namespace ThingSet::Ip::Zsock {

class ThingSetZephyrSocketServerTransport : public ThingSetIpServerTransport<SocketEndpoint>
{
private:
    sockaddr_in _publishAddress;
    sockaddr_in _listenAddress;
    sockaddr_in _broadcastAddress;
    int _publishSocketHandle;
    int _listenSocketHandle;
    k_tid_t _acceptorThreadId;
    k_tid_t _handlerThreadId;
    std::function<int(const SocketEndpoint &, uint8_t *, size_t, uint8_t *, size_t)> _callback;

public:
    ThingSetZephyrSocketServerTransport(net_if *iface);
    ~ThingSetZephyrSocketServerTransport();

    bool listen(std::function<int(const SocketEndpoint &, uint8_t *, size_t, uint8_t *, size_t)> callback) override;
    bool publish(uint8_t *buffer, size_t len) override;

private:
    // TODO: consider https://stackoverflow.com/questions/51451843/creating-a-template-to-wrap-c-member-functions-and-expose-as-c-callbacks

    static void runAcceptor(void *p1, void *, void *);
    void runAcceptor();

    static void runHandler(void *p1, void *, void *);
    void runHandler();
};

} // namespace ThingSet::Ip::Zsock
