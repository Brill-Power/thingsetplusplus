/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "thingset++/ip/StreamingUdpThingSetBinaryDecoder.hpp"

namespace ThingSet::Ip {

MessageType getMessageType(const Frame &message)
{
    return (MessageType)(message.buffer[0] & 0xF0);
}

StreamingUdpThingSetBinaryDecoder::StreamingUdpThingSetBinaryDecoder() : StreamingQueuingThingSetBinaryDecoder<THINGSET_STREAMING_MSG_SIZE, Frame, MessageType, THINGSET_STREAMING_DECODER_UDP_QUEUE_TYPE>()
{}

void StreamingUdpThingSetBinaryDecoder::getBuffer(const Frame &message, const uint8_t **buffer, size_t *length) const
{
    *buffer = &message.buffer[2];
    *length = message.length - 2;
}

size_t StreamingUdpThingSetBinaryDecoder::headerSize() const
{
    return THINGSET_STREAMING_HEADER_SIZE;
}

} // namespace ThingSet::Ip