/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "thingset++/StreamingQueuingThingSetBinaryDecoder.hpp"
#include "thingset++/ip/StreamingUdp.hpp"

#ifdef __ZEPHYR__
#include "thingset++/zephyr/MessageQueue.hpp"
namespace ThingSet::Ip {

template <typename T>
using UdpDecoderMessageQueue = ThingSet::Zephyr::MessageQueue<T, THINGSET_SOCKETS_REPORT_DECODER_QUEUE_SIZE>;

}
#define THINGSET_STREAMING_DECODER_UDP_QUEUE_TYPE UdpDecoderMessageQueue
#else
#define THINGSET_STREAMING_DECODER_UDP_QUEUE_TYPE std::queue
#endif

namespace ThingSet::Ip {

struct Frame
{
    uint8_t buffer[THINGSET_STREAMING_MSG_SIZE];
    size_t length;

    inline uint8_t *getData()
    {
        return buffer;
    }
};

MessageType getMessageType(const Frame &message);

class StreamingUdpThingSetBinaryDecoder : public StreamingQueuingThingSetBinaryDecoder<THINGSET_STREAMING_MSG_SIZE, Frame, MessageType, THINGSET_STREAMING_DECODER_UDP_QUEUE_TYPE>
{
public:
    StreamingUdpThingSetBinaryDecoder();

protected:
    void getBuffer(const Frame &message, const uint8_t **buffer, size_t *length) const override;

    size_t headerSize() const override;
};

} // namespace ThingSet::Ip