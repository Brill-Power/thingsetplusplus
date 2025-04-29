/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "thingset++/zsock/ThingSetZephyrSocketClientTransport.hpp"
#include <cstring>
#include <zephyr/net/net_if.h>
#include <zephyr/net/net_ip.h>
#include <zephyr/net/socket.h>

namespace ThingSet::Zsock {

ThingSetZephyrSocketClientTransport::ThingSetZephyrSocketClientTransport(const char *ip, uint16_t port)
    : _sock(-1), _opt(1), _optlen(sizeof(_opt))
{
    std::memset(&_servaddr, 0, sizeof(_servaddr));
    std::memset(&_cli, 0, sizeof(_cli));

    int ret = 0;
    char hr_addr[NET_IPV4_ADDR_LEN];

    struct net_if *iface = net_if_get_default();
    
    _servaddr.sin_family = AF_INET;
    _servaddr.sin_port = htons(port);
    ret = net_addr_pton(AF_INET, ip, &_servaddr.sin_addr);

    net_if_ipv4_addr_add(iface, &_servaddr.sin_addr, NET_ADDR_MANUAL, 0);

	printk("IPv4 address: %s\n",
		net_addr_ntop(AF_INET, &_servaddr.sin_addr, hr_addr,
			      NET_IPV4_ADDR_LEN));

    printk("RET: %d, %d\n", ret, errno);

    _sock = zsock_socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    ret = zsock_getsockopt(_sock, IPPROTO_IP, SOL_SOCKET, &_opt, (socklen_t *)&_optlen);
    printk("RET: %d, %d\n", ret, errno);
    ret = zsock_bind(_sock, (struct sockaddr *)&_servaddr, sizeof(_servaddr));

    printk("RET: %d, %d\n", ret, errno);

    ret = zsock_listen(_sock, 1);

    printk("RET: %d, %d\n", ret, errno);
}

} // namespace ThingSet::Zsock