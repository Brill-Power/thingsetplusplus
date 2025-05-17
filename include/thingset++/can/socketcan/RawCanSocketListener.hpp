/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "thingset++/can/socketcan/RawCanSocket.hpp"
#include <thread>

namespace ThingSet::Can::SocketCan {

class RawCanSocketListener
{
protected:
    RawCanSocket _socket;
    std::thread _thread;
    bool _run;

public:
    RawCanSocketListener();
    ~RawCanSocketListener();

    void shutdown();
};

} // ThingSet::Can::SocketCan