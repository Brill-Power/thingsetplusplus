/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "thingset++/ThingSetClientTransport.hpp"
#include <cstdint>
#include <cstdio>
#include <zephyr/kernel.h>
#include <zephyr/net/net_if.h>
#include <zephyr/net/socket.h>

namespace ThingSet::Zsock {

class ThingSetZephyrSocketClientTransport : public ThingSetClientTransport
{
    private:
        struct sockaddr_in _nodeaddr;
        struct sockaddr_in _clientaddr;

        struct sockaddr_in _udpaddr;

        int _requestResponseSocket;
        int _subscribeSocket;

        int _listenerThreadID;
        std::function<void(uint8_t *, size_t)> _listenerCallback;
        uint8_t _buffer[1024];

    public:
        ThingSetZephyrSocketClientTransport(struct net_if *iface, const char *ip, uint16_t port);

        /// @note Must call set_client_connection_params before connect
        ///       else an __ASSERT will fail during zsock_connect
        bool connect() override;
        int read(uint8_t *buffer, size_t len) override;
        bool write(uint8_t *buffer, size_t len) override;
        void subscribe(std::function<void(uint8_t *, size_t)> callback) override;

        /// @brief Must be called before connect().
        /// @param ip A null-terminated array containing the IP address of the device to which
        ///           to connect.
        /// @param port The port of the service running on the device to which to connect.
        /// @return True if the connection parameters were successfully set.
        bool set_client_connection_params(const char *ip, uint16_t port);

        uint8_t *get_buffer(void);

        int get_sub_sock(void);

        std::function<void(uint8_t *, size_t)> get_callback(void);
};

} // namespace ThingSet::Zsock
