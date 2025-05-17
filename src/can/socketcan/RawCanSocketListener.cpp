/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "thingset++/can/socketcan/RawCanSocketListener.hpp"

namespace ThingSet::Can::SocketCan {

RawCanSocketListener::RawCanSocketListener()
{
    _run = true;
}

RawCanSocketListener::~RawCanSocketListener()
{
    shutdown();
}

void RawCanSocketListener::shutdown()
{
    _run = false;
    if (_thread.joinable()) {
        _thread.join();
    }
}

} // ThingSet::Can::SocketCan