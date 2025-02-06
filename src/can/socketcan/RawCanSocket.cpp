/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "can/socketcan/RawCanSocket.hpp"
#include <linux/can/raw.h>
#include <net/if.h>

namespace ThingSet::Can::SocketCan {

RawCanSocket::RawCanSocket()
{
    _canSocket = socket(PF_CAN, SOCK_RAW, CAN_RAW);
}

RawCanSocket::~RawCanSocket()
{
    close(_canSocket);
}

bool RawCanSocket::getIsFd()
{
    int enableFd;
    socklen_t size = sizeof(enableFd);
    getsockopt(_canSocket, SOL_CAN_RAW, CAN_RAW_FD_FRAMES, &enableFd, &size);
    return enableFd > 0;
}

RawCanSocket &RawCanSocket::setIsFd(bool value)
{
    int enableFd = (value ? 1 : 0);
    int result = setsockopt(_canSocket, SOL_CAN_RAW, CAN_RAW_FD_FRAMES, &enableFd, sizeof(enableFd));
    return *this;
}

bool RawCanSocket::setFilter(const Can::CanID &canId)
{
    can_filter filter[1] = { {
        .can_id = canId,
        .can_mask = canId.getMask(),
    } };
    int result = setsockopt(_canSocket, SOL_CAN_RAW, CAN_RAW_FILTER, &filter, sizeof(filter));
    return result == 0;
}

bool RawCanSocket::bind(const std::string &deviceName)
{
    int result = 0;
    _address.can_family = AF_CAN;
    _address.can_ifindex = if_nametoindex(deviceName.c_str());

    result = ::bind(_canSocket, (sockaddr *)&_address, sizeof(_address));
    if (result != 0) {
        return false;
    }

    return true;
}

} // namespace ThingSet::Can::SocketCan