/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "ThingSetBinaryEncoder.hpp"

namespace ThingSet {

class _StreamingThingSetBinaryEncoder : public virtual ThingSetBinaryEncoder
{
protected:
    zcbor_state_t _state[BINARY_ENCODER_DEFAULT_MAX_DEPTH];
    size_t _exportedLength;

protected:
    _StreamingThingSetBinaryEncoder() : _exportedLength(0)
    {}

public:
    size_t getEncodedLength() const override
    {
        return _exportedLength;
    }

    bool getIsForwardOnly() const override
    {
        return true;
    }

    template <typename T> bool encodePair(uint32_t id, T &value)
    {
        return encode(id) && encode(value);
    }

    virtual bool flush() = 0;

protected:

    zcbor_state_t *getState() override
    {
        return _state;
    }

    virtual bool write(size_t length, bool flushing) = 0;
};

/// @brief Base class for streaming encoders.
/// @tparam Size The size of the chunks to write.
template <size_t Size> class StreamingThingSetBinaryEncoder : public _StreamingThingSetBinaryEncoder
{
protected:
    std::array<uint8_t, Size * 2> _buffer;

public:
    StreamingThingSetBinaryEncoder() : _StreamingThingSetBinaryEncoder()
    {}

    /// @brief Write remaining buffer to the output.
    bool flush() override
    {
        size_t currentLength = _state->payload - &_buffer[0];
        size_t remainder = _buffer.size() - currentLength;
        memset(&_buffer[currentLength], 0, remainder);
        bool result = write(currentLength, true);
        _exportedLength += currentLength;
        return result;
    }

protected:
    bool ensureState() override
    {
        return writeIfNecessary();
    }

private:
    /// @brief If the buffer contains more than @tparam Size bytes worth of data,
    /// writes the data to the underlying stream.
    /// @return True.
    bool writeIfNecessary()
    {
        size_t currentPos = _state->payload - &_buffer[0];
        if (currentPos > Size) {
            if (!write(Size, false)) {
                // not sure what else to do with this for now
                return false;
            }
            memmove(&_buffer[0], &_buffer[Size],
                    Size);           // move to start
            _exportedLength += Size; // keep track
            size_t newPos = currentPos - Size;
            zcbor_update_state(_state, &_buffer[newPos], _buffer.size() - newPos);
        }
        return true;
    }
};

} // namespace ThingSet
