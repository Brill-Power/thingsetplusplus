/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "thingset++/Streaming.hpp"
#include "thingset++/StreamingThingSetBinaryDecoder.hpp"
#include <queue>

namespace ThingSet {

template <size_t Size, typename Message, StreamingMessageType MessageType, template <typename> typename Queue = std::queue>
class StreamingQueuingThingSetBinaryDecoder : public StreamingThingSetBinaryDecoder<Size>
{
private:
    Queue<Message> _queue;

public:
    using message_type = MessageType;


    StreamingQueuingThingSetBinaryDecoder() : StreamingThingSetBinaryDecoder<Size>(2)
    {}

    bool enqueue(Message &&message) {
        _queue.push(std::move(message));
        MessageType messageType = getMessageType(message);
        // for now, require reception of all frames before marking as ready
        // if we could make the pull parsing blocking, perhaps we can revise this
        if (messageType == MessageType::single || messageType == MessageType::last) {
            // fill buffer
            read(0, Size * 2);
            this->initialiseState(this->_state, BINARY_DECODER_DEFAULT_MAX_DEPTH, &this->_buffer[1], this->_buffer.size() - 1, 2);
            return true;
        }
        return false;
    }

protected:
    int read() override {
        size_t dataSize = Size - this->headerSize();
        return read(dataSize, Size * 2);
    }

    virtual void getBuffer(const Message &message, const uint8_t **buffer, size_t *length) const = 0;

private:
    int read(size_t pos, size_t maxSize) {
        if (_queue.empty()) {
            return 0;
        }
        while (pos <= maxSize && !_queue.empty()) {
            const Message &message = _queue.front();
            const uint8_t *buffer;
            size_t length;
            getBuffer(message, &buffer, &length);
            if (pos + length < maxSize) {
                memcpy(&this->_buffer.data()[pos], buffer, length);
                pos += length;
                _queue.pop();
            } else {
                break;
            }
        }
        return pos;
    }
};

} // namespace ThingSet