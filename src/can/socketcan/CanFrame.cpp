/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "can/socketcan/CanFrame.hpp"
#include <string.h>

namespace ThingSet::Can::SocketCan {

using namespace ThingSet::Can;

CanFrame::CanFrame() : SocketCanFrame<CanFrame, can_frame, CAN_MAX_DLEN>::SocketCanFrame()
{}

CanFrame::CanFrame(CanID &id) : SocketCanFrame<CanFrame, can_frame, CAN_MAX_DLEN>::SocketCanFrame(id)
{}

CanFrame::CanFrame(CanID &id, std::array<uint8_t, CAN_MAX_DLEN> buffer)
    : SocketCanFrame<CanFrame, can_frame, CAN_MAX_DLEN>::SocketCanFrame(id, buffer)
{}

CanFdFrame::CanFdFrame() : SocketCanFrame<CanFdFrame, canfd_frame, CANFD_MAX_DLEN>::SocketCanFrame()
{}

CanFdFrame::CanFdFrame(CanID &id) : SocketCanFrame<CanFdFrame, canfd_frame, CANFD_MAX_DLEN>::SocketCanFrame(id)
{}

CanFdFrame::CanFdFrame(CanID &id, std::array<uint8_t, CANFD_MAX_DLEN> buffer)
    : SocketCanFrame<CanFdFrame, canfd_frame, CANFD_MAX_DLEN>::SocketCanFrame(id, buffer)
{}

} // namespace ThingSet::Can::SocketCan