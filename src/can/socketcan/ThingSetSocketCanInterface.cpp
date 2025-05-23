/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "thingset++/can/socketcan/ThingSetSocketCanInterface.hpp"
#include "thingset++/can/StreamingCanThingSetBinaryDecoder.hpp"
#include "thingset++/can/CanID.hpp"
#include "thingset++/Eui.hpp"
#include "thingset++/ThingSetStatus.hpp"
#include "thingset++/internal/logging.hpp"
#include <chrono>
#include <random>
#include <string.h>

namespace ThingSet::Can::SocketCan {

static bool tryClaimAddress(uint8_t nodeAddress, RawCanSocket &socket)
{
    LOG_DBG("Asserting claim to 0x%x", nodeAddress);
    CanFdFrame frame(CanID()
                       .setSource(nodeAddress)
                       .setTarget(CanID::broadcastAddress)
                       .setMessageType(MessageType::network)
                       .setMessagePriority(MessagePriority::networkManagement),
                   Eui::getArray());
    int result = socket.write(frame);
    return result > 0;
}

void ThingSetSocketCanInterface::AddressClaimer::run(const std::string &deviceName, uint8_t nodeAddress)
{
    LOG_DBG("Starting address claimer for 0x%x", nodeAddress);
    _socket.setFilter(
        CanID().setSource(CanID::anonymousAddress).setTarget(nodeAddress).setMessageType(MessageType::network));
        _socket.bind(deviceName);
    _socket.setIsFd(true);
    auto runner = [&, nodeAddress]() {
        while (_run) {
            CanFrame frame;
            if (_socket.tryRead(frame, std::chrono::milliseconds(500)) > 0) {
                tryClaimAddress(nodeAddress, _socket);
            }
        }
    };
    _thread = std::thread(runner);
}

ThingSetSocketCanInterface::ThingSetSocketCanInterface(const std::string deviceName)
    : _deviceName(deviceName)
{}

ThingSetSocketCanInterface::~ThingSetSocketCanInterface()
{
    _claimer.shutdown();
}

const std::string ThingSetSocketCanInterface::getDeviceName() const
{
    return _deviceName;
}

bool ThingSetSocketCanInterface::bind(uint8_t nodeAddress)
{
    RawCanSocket claimLookoutSocket;
    claimLookoutSocket.setFilter(CanID().setTarget(CanID::broadcastAddress).setMessageType(MessageType::network));
    claimLookoutSocket.bind(_deviceName);
    claimLookoutSocket.setIsFd(true);
    std::random_device dice;
    std::mt19937 rng(dice());
    std::uniform_int_distribution<std::mt19937::result_type> dist(CanID::minAddress, CanID::maxAddress);
    while (true) {
        uint8_t random = (uint8_t)dist(rng);
        CanFdFrame claimFrame(CanID()
                                .setSource(CanID::anonymousAddress)
                                .setTarget(nodeAddress)
                                .setRandomElement(random)
                                .setMessageType(MessageType::network)
                                .setMessagePriority(MessagePriority::networkManagement));
        claimLookoutSocket.write(claimFrame);
        CanFdFrame claimedFrame;
        if (claimLookoutSocket.tryRead(claimedFrame, std::chrono::milliseconds(500))
            && claimedFrame.getId().getSource() == nodeAddress)
        {
            LOG_DBG("0x%x in use; trying another", nodeAddress);
            nodeAddress = (uint8_t)dist(rng);
        }
        else {
            if (!tryClaimAddress(nodeAddress, claimLookoutSocket)) {
                continue;
            }

            LOG_DBG("Claim of 0x%x succeeded", nodeAddress);
            _nodeAddress = nodeAddress;
            break;
        }
    }

    _claimer.run(_deviceName, _nodeAddress);
    return true;
}

} // namespace ThingSet::Can::SocketCan