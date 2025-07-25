/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <cstdint>

namespace ThingSet::Ip {

// minimum MTU of 576 less 20-byte IPv4 header and 8-byte UDP header
// leaving a bit of spare
// https://stackoverflow.com/questions/1098897/what-is-the-largest-safe-udp-packet-size-on-the-internet
#define THINGSET_STREAMING_MSG_SIZE 512
#define THINGSET_STREAMING_HEADER_SIZE 2
#define THINGSET_STREAMING_MESSAGE_TYPE_POS 4
#define THINGSET_STREAMING_MESSAGE_TYPE_MASK 0xF0
#define THINGSET_STREAMING_SEQUENCE_NUM_MASK 0x0F

/// @brief Specifies the sub-type of a frame in a streamed report.
enum struct MessageType : uint8_t
{
    first = 0x0 << THINGSET_STREAMING_MESSAGE_TYPE_POS,
    consecutive = 0x1 << THINGSET_STREAMING_MESSAGE_TYPE_POS,
    last = 0x2 << THINGSET_STREAMING_MESSAGE_TYPE_POS,
    single = 0x3 << THINGSET_STREAMING_MESSAGE_TYPE_POS
};

} // ThingSet::Ip