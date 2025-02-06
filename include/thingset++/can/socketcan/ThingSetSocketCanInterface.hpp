/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "can/ThingSetCanInterface.hpp"
#include "can/socketcan/IsoTpCanSocket.hpp"
#include "can/socketcan/RawCanSocket.hpp"
#include <functional>
#include <string>
#include <thread>

namespace ThingSet::Can::SocketCan {

/// @brief Encapsulates a ThingSet CAN interface.
class ThingSetSocketCanInterface : public ThingSetCanInterface
{
private:
    /// @brief Listens for address claims matching this node's address and
    /// asserts a claim to that address.
    class AddressClaimer
    {
    private:
        RawCanSocket _socket;
        std::thread _thread;
        bool _run;

    public:
        AddressClaimer();
        ~AddressClaimer();
        void run(const std::string &deviceName, uint8_t nodeAddress);
        void shutdown();
    };

private:
    const std::string &_deviceName;
    AddressClaimer _claimer;
    RawCanSocket _publishSocket;
    IsoTpCanSocket::IsoTpCanSocketListener _listener;

public:
    ThingSetSocketCanInterface(const std::string &deviceName);
    ~ThingSetSocketCanInterface();

    using ThingSetCanInterface::bind;
    bool bind(uint8_t nodeAddress) override;

    bool listen(std::function<int(uint8_t *, size_t, uint8_t *, size_t)> callback) override;

    bool publish(CanID &id, uint8_t *buffer, size_t length) override;
};

} // namespace ThingSet::Can::SocketCan