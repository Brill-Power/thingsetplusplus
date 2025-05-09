/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "thingset++/can/socketcan/IsoTpCanSocket.hpp"
#include <linux/can/isotp.h>
#include <net/if.h>
#include <poll.h>
#include <unistd.h>

namespace ThingSet::Can::SocketCan {

#define ISOTP_PCI_TYPE_MASK 0xF0
#define ISOTP_PCI_FF        0x10

IsoTpCanSocket::IsoTpCanSocket()
{
    _canSocket = socket(PF_CAN, SOCK_DGRAM, CAN_ISOTP);
}

IsoTpCanSocket::~IsoTpCanSocket()
{
    close(_canSocket);
}

bool IsoTpCanSocket::getIsFd()
{
    can_isotp_ll_options linkLayerOptions;
    socklen_t size;
    // what if this fails?
    getsockopt(_canSocket, SOL_CAN_ISOTP, CAN_ISOTP_LL_OPTS, &linkLayerOptions, &size);
    return linkLayerOptions.tx_dl == CANFD_MAX_DLEN;
}

IsoTpCanSocket &IsoTpCanSocket::setIsFd(bool value)
{
    can_isotp_ll_options linkLayerOptions = {
        .mtu = (uint8_t)(value ? CANFD_MTU : CAN_MTU),
        .tx_dl = (uint8_t)(value ? CANFD_MAX_DLEN : CAN_MAX_DLEN),
    };
    //.tx_flags = (uint8_t)(value ? CANFD_BRS : 0),
    setsockopt(_canSocket, SOL_CAN_ISOTP, CAN_ISOTP_LL_OPTS, &linkLayerOptions, sizeof(linkLayerOptions));
    return *this;
}

bool IsoTpCanSocket::bind(const std::string &deviceName, const Can::CanID &rxId, const Can::CanID &txId)
{
    sockaddr_can listenAddress = {
        .can_family = AF_CAN,
        .can_ifindex = (int)if_nametoindex(deviceName.c_str()),
        .can_addr = {
            .tp = {
                .rx_id = rxId,
                .tx_id = txId,
            }
        },
    };
    return ::bind(_canSocket, (sockaddr *)&listenAddress, sizeof(listenAddress)) == 0;
}

int IsoTpCanSocket::read(uint8_t *buffer, size_t size)
{
    return ::read(_canSocket, buffer, size);
}

int IsoTpCanSocket::write(uint8_t *buffer, size_t length)
{
    return ::write(_canSocket, buffer, length);
}

IsoTpCanSocket::IsoTpCanSocketListener::IsoTpCanSocketListener(const std::string &deviceName, bool fd)
    : _deviceName(deviceName), _run(true)
{
    _listenSocket.setIsFd(fd);
}

IsoTpCanSocket::IsoTpCanSocketListener::~IsoTpCanSocketListener()
{
    _run = false;
    if (_listenThread.joinable()) {
        _listenThread.join();
    }
}

bool IsoTpCanSocket::IsoTpCanSocketListener::listen(const Can::CanID &address,
                                                    std::function<void(CanID &, IsoTpCanSocket)> callback)
{
    _listenSocket.setFilter(address);
    _listenSocket.bind(_deviceName);

    auto runner = [&]() {
        bool fd = _listenSocket.getIsFd();
        while (_run) {
            CanFdFrame frame;
            int result = _listenSocket.read(frame);
            if (result > 0 && (frame.getData()[0] & ISOTP_PCI_TYPE_MASK) <= ISOTP_PCI_FF) { // SF or CF
                IsoTpCanSocket client;
                client.setIsFd(fd);
                client.bind(_deviceName, frame.getId(), frame.getId().getReplyId());
                // this is a despicable hack: put the frame we just received back on
                // the wire so that the ISO-TP socket we've just created and bound sees it
                _listenSocket.write(frame);
                CanID sender = frame.getId();
                callback(sender, std::move(client));
            }
        }
    };
    _listenThread = std::thread(runner);
    _listenThread.join();
    return true;
}

} // namespace ThingSet::Can::SocketCan