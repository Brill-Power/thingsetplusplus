/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "thingset++/zsock/ThingSetZephyrSocketClientTransport.hpp"
#include <assert.h>
#include <zephyr/net/net_if.h>
#include <zephyr/net/net_ip.h>
#include <zephyr/net/socket.h>

namespace ThingSet::Zsock {

ThingSetZephyrSocketClientTransport::ThingSetZephyrSocketClientTransport(struct net_if *iface, const char *ip, uint16_t port)
    : _sock(-1), _opt(1), _optlen(sizeof(_opt))
{
    _nodeaddr.sin_family = AF_INET;
    _nodeaddr.sin_port = htons(port);

    int ret = net_addr_pton(AF_INET, ip, &_nodeaddr.sin_addr);

    struct net_if_addr *if_addr = net_if_ipv4_addr_add(iface, &_nodeaddr.sin_addr,
                                                       NET_ADDR_MANUAL, 0);

    _sock = zsock_socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    __ASSERT(ret == 0, "Failed to convert %s to IP address: %d", ip, errno);
    __ASSERT(if_addr != NULL, "Failed to add address to interface: %d", errno);
    __ASSERT(_sock >= 0, "Failed to create socket: %d", errno);
}

bool ThingSetZephyrSocketClientTransport::set_client_connection_params(const char *ip, uint16_t port)
{
    _clientaddr.sin_family = AF_INET;
    _clientaddr.sin_port = htons(port);
    
    return net_addr_pton(AF_INET, ip, &_clientaddr.sin_addr) == 0;
}

bool ThingSetZephyrSocketClientTransport::connect()
{
    return zsock_connect(_sock, (struct sockaddr *)&_clientaddr, sizeof(_clientaddr)) == 0;
}

int ThingSetZephyrSocketClientTransport::read(uint8_t *buffer, size_t len)
{
    return zsock_recv(_sock, buffer, len, 0);
}

bool ThingSetZephyrSocketClientTransport::write(uint8_t *buffer, size_t len)
{
    return zsock_send(_sock, buffer, len, 0) == (ssize_t)len;
}

void ThingSetZephyrSocketClientTransport::subscribe(std::function<void(uint8_t *, size_t)> callback)
{
    // asio::ip::udp::endpoint localEndpoint(asio::ip::address_v4::any(), 9002);
    // asio::error_code error;
    // _subscribeSocket.bind(localEndpoint, error);
    // if (error) {
    //     throw std::system_error(error);
    // }
    // co_spawn(_ioContext, listener(callback), detached);
    return;
}

} // namespace ThingSet::Zsock