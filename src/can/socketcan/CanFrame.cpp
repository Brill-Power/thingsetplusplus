/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "thingset++/can/socketcan/CanFrame.hpp"
#include <string.h>

namespace ThingSet::Can::SocketCan {

using namespace ThingSet::Can;

CanFrame::CanFrame() : SocketCanFrame<CanFrame, can_frame, CAN_MAX_DLEN>::SocketCanFrame()
{}

CanFrame::CanFrame(CanID &id) : SocketCanFrame<CanFrame, can_frame, CAN_MAX_DLEN>::SocketCanFrame(id)
{}

CanFdFrame::CanFdFrame() : SocketCanFrame<CanFdFrame, canfd_frame, CANFD_MAX_DLEN>::SocketCanFrame()
{}

CanFdFrame::CanFdFrame(CanID &id) : SocketCanFrame<CanFdFrame, canfd_frame, CANFD_MAX_DLEN>::SocketCanFrame(id)
{}

} // namespace ThingSet::Can::SocketCan