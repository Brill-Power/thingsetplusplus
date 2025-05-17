/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "thingset++/can/ThingSetCanInterface.hpp"
#include "thingset++/can/socketcan/IsoTpCanSocket.hpp"
#include "thingset++/can/socketcan/RawCanSocketListener.hpp"
#include <string>
#include <thread>

namespace ThingSet::Can::SocketCan {

/// @brief Encapsulates a ThingSet CAN interface.
class ThingSetSocketCanInterface : public ThingSetCanInterface
{
private:
    /// @brief Listens for address claims matching this node's address and
    /// asserts a claim to that address.
    class AddressClaimer : public RawCanSocketListener
    {
    public:
        void run(const std::string &deviceName, uint8_t nodeAddress);
    };

private:
    const std::string _deviceName;
    AddressClaimer _claimer;

public:
    ThingSetSocketCanInterface(const ThingSetSocketCanInterface &) = delete;
    ThingSetSocketCanInterface(ThingSetSocketCanInterface &&) = delete;
    ThingSetSocketCanInterface(const std::string deviceName);
    ~ThingSetSocketCanInterface();

    const std::string getDeviceName() const;

    using ThingSetCanInterface::bind;
    bool bind(uint8_t nodeAddress) override;
};

} // namespace ThingSet::Can::SocketCan