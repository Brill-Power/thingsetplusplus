/*
 * Copyright (c) 2024 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */
#pragma once

#include "can/AbstractCanFrame.hpp"
#include "linux/can.h"
#include "linux/can/raw.h"
#include <array>
#include <string.h>

namespace ThingSet::Can::SocketCan {

class CanFrame : public AbstractCanFrame<CanFrame, can_frame, CAN_MAX_DLEN>
{
public:
    CanFrame();
    CanFrame(CanID id);
    CanFrame(CanID id, std::array<uint8_t, CAN_MAX_DLEN> buffer);
};

class CanFdFrame : public AbstractCanFrame<CanFdFrame, canfd_frame, CANFD_MAX_DLEN>
{
public:
    CanFdFrame();
    CanFdFrame(CanID id);
    CanFdFrame(CanID id, std::array<uint8_t, CANFD_MAX_DLEN> buffer);
};

} // namespace ThingSet::Can::SocketCan