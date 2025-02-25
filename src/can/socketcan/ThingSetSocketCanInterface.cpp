/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "can/socketcan/ThingSetSocketCanInterface.hpp"
#include "Eui.hpp"
#include "can/CanID.hpp"
#include "internal/logging.hpp"
#include <chrono>
#include <random>
#include <string.h>

namespace ThingSet::Can::SocketCan {

#define THINGSET_REQUEST_BUFFER_SIZE  1024
#define THINGSET_RESPONSE_BUFFER_SIZE 1024

static bool tryClaimAddress(uint8_t nodeAddress, RawCanSocket &socket)
{
    LOG_DBG("Asserting claim to 0x%x", nodeAddress);
    CanFrame frame(CanID()
                       .setSource(nodeAddress)
                       .setTarget(CanID::broadcastAddress)
                       .setMessageType(MessageType::network)
                       .setMessagePriority(MessagePriority::networkManagement),
                   Eui::getArray());
    int result = socket.write(frame);
    return result > 0;
}

ThingSetSocketCanInterface::AddressClaimer::AddressClaimer()
{
    _run = true;
}

ThingSetSocketCanInterface::AddressClaimer::~AddressClaimer()
{
    shutdown();
}

void ThingSetSocketCanInterface::AddressClaimer::shutdown()
{
    _run = false;
    if (_thread.joinable()) {
        _thread.join();
    }
}

void ThingSetSocketCanInterface::AddressClaimer::run(const std::string &deviceName, uint8_t nodeAddress)
{
    LOG_DBG("Starting address claimer for 0x%x", nodeAddress);
    _socket.setFilter(
        CanID().setSource(CanID::anonymousAddress).setTarget(nodeAddress).setMessageType(MessageType::network));
    _socket.bind(deviceName);
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

ThingSetSocketCanInterface::ThingSetSocketCanInterface(const std::string &deviceName)
    : _deviceName(deviceName), _listener(deviceName, true)
{}

ThingSetSocketCanInterface::~ThingSetSocketCanInterface()
{
    _claimer.shutdown();
}

bool ThingSetSocketCanInterface::bind(uint8_t nodeAddress)
{
    RawCanSocket claimLookoutSocket;
    claimLookoutSocket.setFilter(CanID().setTarget(CanID::broadcastAddress).setMessageType(MessageType::network));
    claimLookoutSocket.bind(_deviceName);
    std::random_device dice;
    std::mt19937 rng(dice());
    std::uniform_int_distribution<std::mt19937::result_type> dist(CanID::minAddress, CanID::maxAddress);
    while (true) {
        uint8_t random = (uint8_t)dist(rng);
        CanFrame claimFrame(CanID()
                                .setSource(CanID::anonymousAddress)
                                .setTarget(nodeAddress)
                                .setRandomElement(random)
                                .setMessageType(MessageType::network)
                                .setMessagePriority(MessagePriority::networkManagement));
        claimLookoutSocket.write(claimFrame);
        CanFrame claimedFrame;
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
    _publishSocket.setIsFd(true);
    _publishSocket.bind(_deviceName);
    return true;
}

bool ThingSetSocketCanInterface::publish(CanID &id, uint8_t *buffer, size_t length)
{
    CanFdFrame frame(id);
    memcpy(frame.getData(), buffer, length);
    frame.setLength(length);
    return _publishSocket.write(frame) > 0;
}

bool ThingSetSocketCanInterface::listen(std::function<int(uint8_t *, size_t, uint8_t *, size_t)> callback)
{
    _listener.listen(CanID()
                         .setMessageType(MessageType::requestResponse)
                         .setMessagePriority(MessagePriority::channel)
                         .setBridge(CanID::defaultBridge)
                         .setTarget(_nodeAddress),
                     [&](auto socket) {
                         uint8_t request[THINGSET_REQUEST_BUFFER_SIZE];
                         int size = socket.read(request, sizeof(request));
                         printf("Got request of size %d bytes\n", size);
                         uint8_t response[THINGSET_RESPONSE_BUFFER_SIZE];
                         int responseLength = callback(request, size, response, sizeof(response));
                         socket.write(response, responseLength);
                         printf("Sent response of size %d bytes\n", responseLength);
                     });
    return true;
}

} // namespace ThingSet::Can::SocketCan