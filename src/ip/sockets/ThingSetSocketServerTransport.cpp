/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "thingset++/ip/sockets/ThingSetSocketServerTransport.hpp"
#include <assert.h>
#include <array>
#include <iostream>

#define MAX_CLIENTS               8

#ifdef __ZEPHYR__
#include "thingset++/ip/sockets/ZephyrStubs.h"
#include <zephyr/kernel.h>

#define ACCEPT_THREAD_STACK_SIZE  1024
#define ACCEPT_THREAD_PRIORITY    3
#define HANDLER_THREAD_STACK_SIZE 4096
#define HANDLER_THREAD_PRIORITY   2

K_THREAD_STACK_DEFINE(accept_thread_stack, ACCEPT_THREAD_STACK_SIZE);
static struct k_thread accept_thread;

K_THREAD_STACK_DEFINE(handler_thread_stack, HANDLER_THREAD_STACK_SIZE);
static struct k_thread handler_thread;
#else
#include "thingset++/ip/InterfaceInfo.hpp"
#include <poll.h>
#include <netinet/in.h>
#include <unistd.h>
#define __ASSERT(test, fmt, ...) { if (!(test)) { throw std::invalid_argument(fmt); } }
#endif // __ZEPHYR__

struct PollDescriptor : public pollfd {
public:
    PollDescriptor()
    {
        fd = -1;
        events = POLLIN;
    }
};

static std::array<PollDescriptor, MAX_CLIENTS> sockfd_tcp;

namespace ThingSet::Ip::Sockets {

_ThingSetSocketServerTransport::_ThingSetSocketServerTransport(const std::pair<in_addr, in_addr> &ipAddressAndSubnet)
    : _publishSocketHandle(-1), _listenSocketHandle(-1)
{
    // calculate broadcast address
    _broadcastAddress.sin_family = AF_INET;
    _broadcastAddress.sin_port = htons(9002);
    _broadcastAddress.sin_addr = {
        .s_addr = ipAddressAndSubnet.first.s_addr | (~ipAddressAndSubnet.second.s_addr)
    };

    // local address of publish socket
    _publishAddress.sin_addr = ipAddressAndSubnet.first;
    _publishAddress.sin_family = AF_INET;
    _publishAddress.sin_port = 0;

    _publishSocketHandle = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    __ASSERT(_publishSocketHandle >= 0, "Failed to create UDP socket: %d", errno);

    int optionValue = 1;
    int ret = setsockopt(_publishSocketHandle, SOL_SOCKET, SO_REUSEADDR, &optionValue, sizeof(optionValue));
    __ASSERT(ret == 0, "Failed to configure UDP socket: %d", errno);
#ifndef __ZEPHYR__
    ret = setsockopt(_publishSocketHandle, SOL_SOCKET, SO_BROADCAST, &optionValue, sizeof(optionValue));
    __ASSERT(ret == 0, "Failed to configure UDP socket: %d", errno);
#endif

    // local address of listener
    _listenAddress.sin_addr = ipAddressAndSubnet.first;
    _listenAddress.sin_family = AF_INET;
    _listenAddress.sin_port = htons(9001);

    _listenSocketHandle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    __ASSERT(_listenSocketHandle >= 0, "Failed to create TCP socket: %d", errno);
}

_ThingSetSocketServerTransport::~_ThingSetSocketServerTransport()
{
    close(_publishSocketHandle);
    close(_listenSocketHandle);
    _publishSocketHandle = -1;
    _listenSocketHandle = -1;
}

bool _ThingSetSocketServerTransport::listen(std::function<int(const SocketEndpoint &, uint8_t *, size_t, uint8_t *, size_t)> callback)
{
    if (bind(_publishSocketHandle, (struct sockaddr *)&_publishAddress, sizeof(_publishAddress))) {
        return false;
    }

    if (bind(_listenSocketHandle, (struct sockaddr *)&_listenAddress, sizeof(_listenAddress))) {
        return false;
    }

    _callback = callback;
    startThreads();
    return true;
}

bool _ThingSetSocketServerTransport::publish(uint8_t *buffer, size_t len)
{
    sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    if (getsockname(_publishSocketHandle, (struct sockaddr *)&addr, &addr_len) != 0) {
        return false;
    }

    // sin_port will be non-zero if currently bound, socket must be bound to send
    if (addr.sin_port == 0) {
        return false;
    }

    ssize_t sent = sendto(_publishSocketHandle, buffer, len, 0, (struct sockaddr *)&_broadcastAddress, sizeof(_broadcastAddress));
    if (sent < 0) {
        printf("Failed to send report: %zd %d\n", sent, errno);
    }
    return sent == (ssize_t)len;
}


void _ThingSetSocketServerTransport::runAcceptor()
{
    if (::listen(_listenSocketHandle, MAX_CLIENTS) != 0) {
        printf("Failed to begin listening: %d\n", errno);
        return;
    }

    while (true) {
        sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        char client_addr_str[INET_ADDRSTRLEN];

        int client_sock = accept(_listenSocketHandle, (sockaddr *)&client_addr, &client_addr_len);
        if (client_sock < 0) {
            printf("Accept failed: %d\n", errno);
            continue;
        }

        inet_ntop(client_addr.sin_family, &client_addr.sin_addr, client_addr_str,
                        sizeof(client_addr_str));
        printf("Connection from %s\n", client_addr_str);

        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (sockfd_tcp[i].fd == -1) {
                sockfd_tcp[i].fd = client_sock;
                printf("Assigned slot %d\n", i);
                break;
            }
        }
    }
}

void _ThingSetSocketServerTransport::runHandler()
{
    while (true) {
        int ret = poll(sockfd_tcp.data(), sockfd_tcp.size(), 10);

        if (ret < 0) {
            printf("Polling error: %d\n", errno);
        }
        else if (ret == 0) {
            continue;
        }

        SocketEndpoint addr;
        socklen_t len = sizeof(addr);
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (sockfd_tcp[i].revents & POLLIN) {
                int client_sock = sockfd_tcp[i].fd;
                int rx_len = 0;
                uint8_t rx_buf[1024];
                uint8_t tx_buf[1024];

                rx_len = recv(client_sock, rx_buf, sizeof(rx_buf), 0);

                if (rx_len < 0) {
                    printf("Receive error: %d\n", errno);
                }
                else if (rx_len == 0) {
                    char ip[INET_ADDRSTRLEN];

                    getsockname(client_sock, (sockaddr *)&addr, &len);
                    inet_ntop(AF_INET, &addr.sin_addr, ip, sizeof(ip));

                    //printf("Closing connection from %s\n", ip);
                    std::cout << "Closing connection from " << addr << std::endl;

                    close(client_sock);
                    sockfd_tcp[i].fd = -1;
                }
                else {
                    getsockname(client_sock, (sockaddr *)&addr, &len);
                    auto tx_len = _callback(addr, rx_buf, rx_len, tx_buf, sizeof(tx_buf));
                    send(client_sock, tx_buf, tx_len, 0);
                }
            }
        }
    }
}

#ifdef __ZEPHYR__
static std::pair<in_addr, in_addr> getIpAndSubnetForInterface(net_if *iface)
{
    // find IP address associated with interface
    net_if_ipv4 *ipConfig;
    int ret = net_if_config_ipv4_get(iface, &ipConfig);
    __ASSERT(ret == 0, "Failed to get interface IP information: %d", ret);
    return std::make_pair(ipConfig->unicast->ipv4.address.in_addr, ipConfig->unicast->netmask);
}

ThingSetSocketServerTransport::ThingSetSocketServerTransport(net_if *iface) : _ThingSetSocketServerTransport(getIpAndSubnetForInterface(iface))
{}

void ThingSetSocketServerTransport::startThreads()
{
    _handlerThreadId = k_thread_create(&handler_thread, handler_thread_stack, K_THREAD_STACK_SIZEOF(handler_thread_stack),
                                       runHandler, this, NULL, NULL, HANDLER_THREAD_PRIORITY, 0, K_NO_WAIT);

    _acceptorThreadId = k_thread_create(&accept_thread, accept_thread_stack, K_THREAD_STACK_SIZEOF(accept_thread_stack),
                                        runAcceptor, this, NULL, NULL, ACCEPT_THREAD_PRIORITY, 0, K_NO_WAIT);
}

void ThingSetSocketServerTransport::runAcceptor(void *p1, void *, void *)
{
    ThingSetSocketServerTransport *transport = static_cast<ThingSetSocketServerTransport *>(p1);
    transport->runAcceptor();
}

void ThingSetSocketServerTransport::runHandler(void *p1, void *, void *)
{
    ThingSetSocketServerTransport *transport = static_cast<ThingSetSocketServerTransport *>(p1);
    transport->runHandler();
}
#else
static std::pair<in_addr, in_addr> getDefaultIpAndSubnet()
{
    in_addr ipAddress = {
        .s_addr = 0x0,
    };
    in_addr subnet = {
        .s_addr = 0x00000000,
    };
    return std::make_pair(ipAddress, subnet);
}

static std::pair<in_addr, in_addr> getIpAndSubnetForInterface(const std::string &interface)
{
    in_addr address;
    in_addr subnet;
    InterfaceInfo::get(interface, address, subnet);
    return std::make_pair(address, subnet);
}

ThingSetSocketServerTransport::ThingSetSocketServerTransport() : _ThingSetSocketServerTransport(getDefaultIpAndSubnet())
{}

ThingSetSocketServerTransport::ThingSetSocketServerTransport(const std::string &interface) : _ThingSetSocketServerTransport(getIpAndSubnetForInterface(interface))
{}

void ThingSetSocketServerTransport::startThreads()
{
    _handlerThread = std::thread([&]()
    {
        runHandler();
    });
    _acceptorThread = std::thread([&]()
    {
        runAcceptor();
    });
}
#endif // __ZEPHYR__

} // namespace ThingSet::Ip::Sockets