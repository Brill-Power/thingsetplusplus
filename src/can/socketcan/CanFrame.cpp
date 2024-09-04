/*
 * Copyright (c) 2024 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */
#include "can/socketcan/CanFrame.hpp"
#include <string.h>

namespace ThingSet::Can::SocketCan {

using namespace ThingSet::Can;

CanFrame::CanFrame() : AbstractCanFrame<CanFrame, can_frame, CAN_MAX_DLEN>::AbstractCanFrame()
{}

CanFrame::CanFrame(CanID id) : AbstractCanFrame<CanFrame, can_frame, CAN_MAX_DLEN>::AbstractCanFrame(id)
{}

CanFrame::CanFrame(CanID id, std::array<uint8_t, CAN_MAX_DLEN> buffer)
    : AbstractCanFrame<CanFrame, can_frame, CAN_MAX_DLEN>::AbstractCanFrame(id, buffer)
{}

CanFdFrame::CanFdFrame() : AbstractCanFrame<CanFdFrame, canfd_frame, CANFD_MAX_DLEN>::AbstractCanFrame()
{}

CanFdFrame::CanFdFrame(CanID id) : AbstractCanFrame<CanFdFrame, canfd_frame, CANFD_MAX_DLEN>::AbstractCanFrame(id)
{}

CanFdFrame::CanFdFrame(CanID id, std::array<uint8_t, CANFD_MAX_DLEN> buffer)
    : AbstractCanFrame<CanFdFrame, canfd_frame, CANFD_MAX_DLEN>::AbstractCanFrame(id, buffer)
{}

} // namespace ThingSet::Can::SocketCan