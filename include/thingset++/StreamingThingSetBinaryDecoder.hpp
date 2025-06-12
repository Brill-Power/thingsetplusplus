/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "ThingSetBinaryDecoder.hpp"

namespace ThingSet {

/// @brief Base class for streaming decoders.
/// @tparam Size Size of the chunks which should be read from the underlying stream.
template <size_t Size> class StreamingThingSetBinaryDecoder : public virtual ThingSetBinaryDecoder
{
private:
size_t _decodedLength;

protected:
    zcbor_state_t _state[BINARY_DECODER_DEFAULT_MAX_DEPTH];
    std::array<uint8_t, Size * 2> _buffer;

public:
    StreamingThingSetBinaryDecoder(size_t elementCount)
        : StreamingThingSetBinaryDecoder(elementCount, ThingSetBinaryDecoderOptions{})
    {}

    StreamingThingSetBinaryDecoder(size_t elementCount, ThingSetBinaryDecoderOptions options)
        : ThingSetBinaryDecoder(options), _decodedLength(0)
    {
        initialiseState(_state, BINARY_DECODER_DEFAULT_MAX_DEPTH, &_buffer[0], _buffer.size(), elementCount);
    }

    bool skip() override
    {
        switch (this->peekType()) {
            case ZCBOR_MAJOR_TYPE_LIST:
                return decodeList([&](size_t) { return zcbor_any_skip(this->getState(), NULL); });
            case ZCBOR_MAJOR_TYPE_MAP: {
                if (!zcbor_list_start_decode(this->getState())) {
                    return false;
                }
                size_t entryCount = this->getState()->elem_count / 2;
                for (size_t i = 0; i < entryCount; i++) {
                    // skip key
                    if (!zcbor_any_skip(this->getState(), NULL)) {
                        return false;
                    }

                    // skip value
                    if (!zcbor_any_skip(this->getState(), NULL)) {
                        return false;
                    }
                }
                return zcbor_list_end_decode(this->getState());
            }
            default:
                // hope
                return zcbor_any_skip(this->getState(), NULL);
        }
    }

protected:
    zcbor_state_t *getState() override
    {
        ensureLength();
        return _state;
    }

    bool getIsForwardOnly() const override
    {
        return true;
    }

    size_t getDecodedLength() override
    {
        return _decodedLength;
    }

    virtual int read() = 0;

private:
    bool ensureLength()
    {
        size_t currentPos = _state->payload - &_buffer[0];
        if (currentPos > Size) {
            memmove(&_buffer[0], &_buffer[Size], Size);
            _decodedLength += Size;
            int readSize = read();
            size_t newPos = currentPos - Size;
            zcbor_update_state(_state, &_buffer[newPos], Size + readSize - newPos);
        }
        return true;
    }
};

} // namespace ThingSet