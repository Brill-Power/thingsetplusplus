/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "thingset++/can/socketcan/CanFrame.hpp"
#include "thingset++/can/CanID.hpp"
#include <chrono>
#include <linux/can.h>
#include <poll.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

namespace ThingSet::Can::SocketCan {

class RawCanSocket
{
private:
    int _canSocket;
    sockaddr_can _address;

public:
    RawCanSocket();
    ~RawCanSocket();

    bool getIsFd();
    RawCanSocket &setIsFd(bool value);

    bool setFilter(const CanID &canId);

    bool bind(const std::string &deviceName);

    template <typename Frame> int read(Frame &frame)
    {
        int size = ::read(_canSocket, frame.getFrame(), Frame::size());
        return size;
    }

    template <typename Frame> int write(Frame &frame)
    {
        int size = ::write(_canSocket, frame.getFrame(), Frame::size());
        return size;
    }

    template <typename Frame> int tryRead(Frame &frame, const std::chrono::milliseconds &timeout)
    {
        pollfd requests[] = { {
            .fd = _canSocket,
            .events = POLLIN,
            .revents = 0,
        } };
        int result = ::poll(requests, 1, timeout.count());
        if (result <= 0) {
            return result;
        }
        return read(frame);
    }
};

} // namespace ThingSet::Can::SocketCan