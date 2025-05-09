/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "thingset++/can/socketcan/RawCanSocket.hpp"
#include "thingset++/can/CanID.hpp"
#include <chrono>
#include <functional>
#include <string>
#include <sys/socket.h>
#include <thread>

namespace ThingSet::Can::SocketCan {

class IsoTpCanSocket
{
public:
    class IsoTpCanSocketListener
    {
    private:
        RawCanSocket _listenSocket;
        std::thread _listenThread;
        const std::string _deviceName;
        bool _run;

    public:
        IsoTpCanSocketListener(const std::string &deviceName, bool fd);
        ~IsoTpCanSocketListener();

        bool listen(const Can::CanID &address, std::function<void(CanID &, IsoTpCanSocket)> callback);
    };

private:
    int _canSocket;
    sockaddr_can _address;

public:
    IsoTpCanSocket();
    ~IsoTpCanSocket();

    bool getIsFd();
    IsoTpCanSocket &setIsFd(bool value);

    int read(uint8_t *buffer, size_t size);
    int write(uint8_t *buffer, size_t length);

    bool bind(const std::string &deviceName, const CanID &rxId, const CanID &txId);
};

} // namespace ThingSet::Can::SocketCan