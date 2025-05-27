/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "thingset++/ip/sockets/ThingSetSocketServerTransport.hpp"
#include <assert.h>
#include <array>
#include <iostream>

#ifdef __ZEPHYR__
#include "thingset++/ip/sockets/ZephyrStubs.h"
#include <zephyr/kernel.h>

#define ACCEPT_THREAD_STACK_SIZE  1024
#define ACCEPT_THREAD_PRIORITY    3
#define HANDLER_THREAD_STACK_SIZE 4096
#define HANDLER_THREAD_PRIORITY   2

K_THREAD_STACK_DEFINE(acceptThreadStack, ACCEPT_THREAD_STACK_SIZE);
static struct k_thread acceptThread;

K_THREAD_STACK_DEFINE(handlerThreadStack, HANDLER_THREAD_STACK_SIZE);
static struct k_thread handlerThread;
#else
#include "thingset++/ip/InterfaceInfo.hpp"
#include <netinet/in.h>
#include <unistd.h>
#define __ASSERT(test, fmt, ...) { if (!(test)) { throw std::invalid_argument(fmt); } }
#endif // __ZEPHYR__

namespace ThingSet::Ip::Sockets {

_ThingSetSocketServerTransport::PollDescriptor::PollDescriptor()
{
    fd = -1;
    events = POLLIN;
}

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
    __ASSERT(_publishSocketHandle >= 0, "Failed to create publish socket: %d", errno);

    int optionValue = 1;
    int ret = setsockopt(_publishSocketHandle, SOL_SOCKET, SO_REUSEADDR, &optionValue, sizeof(optionValue));
    __ASSERT(ret == 0, "Failed to configure publish socket: %d", errno);
#ifndef __ZEPHYR__
    ret = setsockopt(_publishSocketHandle, SOL_SOCKET, SO_BROADCAST, &optionValue, sizeof(optionValue));
    __ASSERT(ret == 0, "Failed to configure publish socket: %d", errno);
#endif

    // local address of listener
    _listenAddress.sin_addr = ipAddressAndSubnet.first;
    _listenAddress.sin_family = AF_INET;
    _listenAddress.sin_port = htons(9001);

    _listenSocketHandle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    __ASSERT(_listenSocketHandle >= 0, "Failed to create listen socket: %d", errno);
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
    socklen_t addrLen = sizeof(addr);
    if (getsockname(_publishSocketHandle, (struct sockaddr *)&addr, &addrLen) != 0) {
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
    if (::listen(_listenSocketHandle, THINGSET_SERVER_MAX_CLIENTS) != 0) {
        printf("Failed to begin listening: %d\n", errno);
        return;
    }

    while (true) {
        SocketEndpoint clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);

        int client_sock = accept(_listenSocketHandle, (sockaddr *)&clientAddr, &clientAddrLen);
        if (client_sock < 0) {
            printf("Accept failed: %d\n", errno);
            continue;
        }

        std::cout << "Connection from " << clientAddr << std::endl;

        for (int i = 0; i < THINGSET_SERVER_MAX_CLIENTS; i++) {
            if (_socketDescriptors[i].fd == -1) {
                _socketDescriptors[i].fd = client_sock;
                printf("Assigned slot %d to socket %d\n", i, _socketDescriptors[i].fd);
                break;
            }
        }
    }
}

void _ThingSetSocketServerTransport::runHandler()
{
    while (true) {
        int ret = poll(_socketDescriptors.data(), _socketDescriptors.size(), 10);

        if (ret < 0) {
            printf("Polling error: %d\n", errno);
        }
        else if (ret == 0) {
            continue;
        }

        SocketEndpoint addr;
        socklen_t len = sizeof(addr);
        for (int i = 0; i < THINGSET_SERVER_MAX_CLIENTS; i++) {
            if (_socketDescriptors[i].revents & POLLIN) {
                int clientSocketHandle = _socketDescriptors[i].fd;
                uint8_t rxBuf[1024];
                uint8_t txBuf[1024];

                int rxLen = recv(clientSocketHandle, rxBuf, sizeof(rxBuf), 0);

                if (rxLen < 0) {
                    printf("Receive error: %d\n", errno);
                }
                else if (rxLen == 0) {
                    getpeername(clientSocketHandle, (sockaddr *)&addr, &len);
                    std::cout << "Closing connection from " << addr << std::endl;

                    close(clientSocketHandle);
                    _socketDescriptors[i].fd = -1;
                }
                else {
                    getpeername(clientSocketHandle, (sockaddr *)&addr, &len);
                    int txLen = _callback(addr, rxBuf, rxLen, txBuf, sizeof(txBuf));
                    if (txLen > 0) {
                        std::cout << "Sending response to " << addr << " of size " << txLen << std::endl;
                        send(clientSocketHandle, txBuf, txLen, 0);
                    }
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
    _handlerThreadId = k_thread_create(&handlerThread, handlerThreadStack, K_THREAD_STACK_SIZEOF(handlerThreadStack),
                                       runHandler, this, NULL, NULL, HANDLER_THREAD_PRIORITY, 0, K_NO_WAIT);

    _acceptorThreadId = k_thread_create(&acceptThread, acceptThreadStack, K_THREAD_STACK_SIZEOF(acceptThreadStack),
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
static std::pair<in_addr, in_addr> getLocalIpAndSubnet()
{
    in_addr ipAddress = {
        .s_addr = 0x0100007f,
    };
    in_addr subnet = {
        .s_addr = 0x000000ff,
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

ThingSetSocketServerTransport::ThingSetSocketServerTransport() : _ThingSetSocketServerTransport(getLocalIpAndSubnet())
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