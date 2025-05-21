/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "thingset++/ip/ThingSetIpServerTransport.hpp"
#include <cstdint>
#include <cstdio>
#include <zephyr/kernel.h>
#include <zephyr/net/net_if.h>
#include <zephyr/net/socket.h>

namespace ThingSet::Ip::Zsock {

class ThingSetZephyrSocketServerTransport : public ThingSetIpServerTransport<in_addr>
{
private:
    sockaddr_in _pub_addr;
    sockaddr_in _req_addr;
    int _pub_sock;
    int _req_sock;
    k_tid_t _accept_tid;
    k_tid_t _handler_tid;
    std::function<int(const in_addr &, uint8_t *, size_t, uint8_t *, size_t)> _callback;

public:
    ThingSetZephyrSocketServerTransport(net_if *iface, const char *ip);
    ~ThingSetZephyrSocketServerTransport();

    bool listen(std::function<int(const in_addr &, uint8_t *, size_t, uint8_t *, size_t)> callback) override;
    bool publish(uint8_t *buffer, size_t len) override;

private:
    // TODO: consider https://stackoverflow.com/questions/51451843/creating-a-template-to-wrap-c-member-functions-and-expose-as-c-callbacks

    static void runAcceptor(void *p1, void *, void *);
    void runAcceptor();

    static void runHandler(void *p1, void *, void *);
    void runHandler();
};

} // namespace ThingSet::Ip::Zsock
