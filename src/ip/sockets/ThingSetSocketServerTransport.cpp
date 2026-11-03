/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "thingset++/ip/sockets/ThingSetSocketServerTransport.hpp"
#include "thingset++/internal/logging.hpp"
#include <assert.h>
#include <array>

#ifdef __ZEPHYR__
#include "thingset++/ip/sockets/ZephyrStubs.h"
#include <zephyr/kernel.h>
#include <zephyr/posix/fcntl.h>

#define FCNTL zsock_fcntl

K_THREAD_STACK_DEFINE(acceptThreadStack, CONFIG_THINGSET_PLUS_PLUS_SOCKET_ACCEPT_THREAD_STACK_SIZE);
static struct k_thread acceptThread;

K_THREAD_STACK_DEFINE(handlerThreadStack, CONFIG_THINGSET_PLUS_PLUS_SOCKET_HANDLER_THREAD_STACK_SIZE);
static struct k_thread handlerThread;
#else
#include "thingset++/ip/InterfaceInfo.hpp"
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>

#define __ASSERT(test, fmt, ...) { if (!(test)) { throw std::invalid_argument(fmt); } }
#define FCNTL fcntl
#endif // __ZEPHYR__

namespace ThingSet::Ip::Sockets {

#if THINGSET_PLUS_PLUS_SOCKET_SERVER_KEEPALIVE
static void configureKeepAlive(int sock)
{
    int enable = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &enable, sizeof(enable)) != 0) {
        LOG_WARN("Failed to enable TCP keep-alive: %d", errno);
        return;
    }
#if defined(TCP_KEEPIDLE) && defined(TCP_KEEPINTVL) && defined(TCP_KEEPCNT)
    int idle = THINGSET_PLUS_PLUS_SOCKET_SERVER_KEEPALIVE_IDLE;
    int interval = THINGSET_PLUS_PLUS_SOCKET_SERVER_KEEPALIVE_INTERVAL;
    int count = THINGSET_PLUS_PLUS_SOCKET_SERVER_KEEPALIVE_COUNT;
    if (setsockopt(sock, IPPROTO_TCP, TCP_KEEPIDLE, &idle, sizeof(idle)) != 0
        || setsockopt(sock, IPPROTO_TCP, TCP_KEEPINTVL, &interval, sizeof(interval)) != 0
        || setsockopt(sock, IPPROTO_TCP, TCP_KEEPCNT, &count, sizeof(count)) != 0)
    {
        LOG_WARN("Failed to configure TCP keep-alive timing (using stack defaults): %d", errno);
    }
#endif
}
#endif // THINGSET_PLUS_PLUS_SOCKET_SERVER_KEEPALIVE

_ThingSetSocketServerTransport::PollDescriptor::PollDescriptor()
{
    fd = -1;
    events = POLLIN;
}

_ThingSetSocketServerTransport::_ThingSetSocketServerTransport(const std::pair<in_addr, in_addr> &ipAddressAndSubnet)
    : _publishSocketHandle(-1), _listenSocketHandle(-1), _runHandler(true), _runAcceptor(true)
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
    [[maybe_unused]] int ret = setsockopt(_publishSocketHandle, SOL_SOCKET, SO_REUSEADDR, &optionValue, sizeof(optionValue));
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
    ret = setsockopt(_listenSocketHandle, SOL_SOCKET, SO_REUSEADDR, &optionValue, sizeof(optionValue));
    __ASSERT(ret == 0, "Failed to configure listen socket: %d", errno);
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

#if THINGSET_PLUS_PLUS_SOCKET_SERVER_KEEPALIVE
    configureKeepAlive(_listenSocketHandle);
#endif

    _callback = callback;
    startThreads();
    return true;
}

bool _ThingSetSocketServerTransport::publish(uint8_t *buffer, size_t len)
{
    buffer[1] = _messageNumber;
    MessageType messageType = (MessageType)(buffer[0] & 0xF0);
    if (messageType == MessageType::single || messageType == MessageType::last) {
        _messageNumber++;
    }

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
        LOG_ERROR("Failed to send report: %zd %d", sent, errno);
    }
    return sent == (ssize_t)len;
}


void _ThingSetSocketServerTransport::runAcceptor()
{
    if (FCNTL(_listenSocketHandle, F_SETFL, O_NONBLOCK) != 0)  {
        LOG_ERROR("Failed to configure socket: %d", errno);
        // this isn't technically a fatal error; it just means we
        // won't get a clean shutdown, because we'll never be able
        // to break out of the acceptor loop
    }

    if (::listen(_listenSocketHandle, THINGSET_SERVER_MAX_CLIENTS) != 0) {
        LOG_ERROR("Failed to begin listening: %d", errno);
        return;
    }

    PollDescriptor listenPoll[1];
    listenPoll->fd = _listenSocketHandle;

    while (_runAcceptor) {
        int ret = poll(listenPoll, 1, 10);
        if (ret < 0) {
            LOG_ERROR("Polling error: %d", errno);
        }
        else if (ret == 0) {
            continue;
        }

        SocketEndpoint clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);

        int client_sock = accept(_listenSocketHandle, (sockaddr *)&clientAddr, &clientAddrLen);
        if (client_sock < 0) {
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                LOG_ERROR("Accept failed: %d", errno);
            }
            continue;
        }

        //std::cout << "Connection from " << clientAddr << std::endl;

        bool assigned = false;
        for (int i = 0; i < THINGSET_SERVER_MAX_CLIENTS; i++) {
            if (_socketDescriptors[i].fd == -1) {
                _socketDescriptors[i].fd = client_sock;
                LOG_DEBUG("Assigned slot %d to socket %d", i, _socketDescriptors[i].fd);
                assigned = true;
                break;
            }
        }
        if (!assigned) {
            // Every slot is taken, previously the accepted socket was dropped but
            // now we refuse it properly
            LOG_WARN("No free client slot (%d in use); refusing connection", THINGSET_SERVER_MAX_CLIENTS);
            close(client_sock);
        }
    }

    LOG_INFO("Shut down acceptor thread");
}

static void setBlocking(int fd, bool val)
{
	int fl;

	fl = FCNTL(fd, F_GETFL, 0);
    __ASSERT(fl != -1, "fcntl(F_GETFL): %d", errno);

	if (val) {
		fl &= ~O_NONBLOCK;
	} else {
		fl |= O_NONBLOCK;
	}

	FCNTL(fd, F_SETFL, fl);
    __ASSERT(fl != -1, "fcntl(F_SETFL): %d", errno);
}

void _ThingSetSocketServerTransport::runHandler()
{
    while (_runHandler) {
        int ret = poll(_socketDescriptors.data(), _socketDescriptors.size(), 10);

        if (ret < 0) {
            LOG_ERROR("Polling error: %d", errno);
        }
        else if (ret == 0) {
            continue;
        }

        SocketEndpoint addr;
        socklen_t len = sizeof(addr);
        for (int i = 0; i < THINGSET_SERVER_MAX_CLIENTS; i++) {
            short revents = _socketDescriptors[i].revents;
            if (revents == 0) {
                continue;
            }
            int clientSocketHandle = _socketDescriptors[i].fd;

            if (!(revents & POLLIN)) {
                // Error or hang-up with nothing left to read (i.e., due to
                // dropped keepalive probes) so free it and return it
                LOG_DEBUG("Socket %d in slot %d signalled 0x%x; closing", clientSocketHandle, i, revents);
                releaseSlot(i);
                continue;
            }

            {
                int rxLen = recv(clientSocketHandle, _rxBuf, sizeof(_rxBuf), 0);

                if (rxLen < 0) {
                    if (errno == EAGAIN || errno == EWOULDBLOCK) {
                        continue;
                    }
                    LOG_ERROR("Receive error on socket %d: %d; closing", clientSocketHandle, errno);
                    releaseSlot(i);
                }
                else if (rxLen == 0) {
                    getpeername(clientSocketHandle, (sockaddr *)&addr, &len);
                    //std::cout << "Closing connection from " << addr << std::endl;

                    releaseSlot(i);
                }
                else {
                    getpeername(clientSocketHandle, (sockaddr *)&addr, &len);
                    int txLen = _callback(addr, _rxBuf, rxLen, _txBuf, sizeof(_txBuf));
                    if (txLen > 0) {
                        setBlocking(clientSocketHandle, true);
                        int sent;
                        for (uint8_t *txB = _txBuf; txLen; txLen -= sent) {
                            sent = send(clientSocketHandle, txB, txLen, 0);
                            if (sent < 0) {
                                LOG_ERROR("Send to %x failed with error %d", addr.sin_addr.s_addr, errno);
                                break;
                            }
                            txB += sent;
                        }
                        setBlocking(clientSocketHandle, false);
                    }
                }
            }
        }
    }

    LOG_INFO("Shut down handler thread");
}

void _ThingSetSocketServerTransport::releaseSlot(size_t slot)
{
    close(_socketDescriptors[slot].fd);
    _socketDescriptors[slot].fd = -1;
    _socketDescriptors[slot].revents = 0;
}

#ifdef __ZEPHYR__
static std::pair<in_addr, in_addr> getIpAndSubnetForInterface(net_if *iface)
{
    // find IP address associated with interface
    net_if_ipv4 *ipConfig;
    [[maybe_unused]] int ret = net_if_config_ipv4_get(iface, &ipConfig);
    __ASSERT(ret == 0, "Failed to get interface IP information: %d", ret);
    return std::make_pair(ipConfig->unicast->ipv4.address.in_addr, ipConfig->unicast->netmask);
}

ThingSetSocketServerTransport::ThingSetSocketServerTransport() : ThingSetSocketServerTransport(net_if_get_default())
{}

ThingSetSocketServerTransport::ThingSetSocketServerTransport(net_if *iface) : _ThingSetSocketServerTransport(getIpAndSubnetForInterface(iface))
{}

void ThingSetSocketServerTransport::startThreads()
{
    _handlerThreadId = k_thread_create(&handlerThread, handlerThreadStack, K_THREAD_STACK_SIZEOF(handlerThreadStack),
                                       runHandler, this, NULL, NULL, CONFIG_THINGSET_PLUS_PLUS_SOCKET_HANDLER_THREAD_PRIORITY, 0, K_NO_WAIT);

    _acceptorThreadId = k_thread_create(&acceptThread, acceptThreadStack, K_THREAD_STACK_SIZEOF(acceptThreadStack),
                                        runAcceptor, this, NULL, NULL, CONFIG_THINGSET_PLUS_PLUS_SOCKET_ACCEPT_THREAD_PRIORITY, 0, K_NO_WAIT);
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

ThingSetSocketServerTransport::~ThingSetSocketServerTransport()
{
    _runHandler = false;
    _runAcceptor = false;

    if (_handlerThread.joinable()) {
        _handlerThread.join();
    }
    if (_acceptorThread.joinable()) {
        _acceptorThread.join();
    }
}

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