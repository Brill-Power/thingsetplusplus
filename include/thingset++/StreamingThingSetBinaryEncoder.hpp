/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "thingset++/ThingSetBinaryEncoder.hpp"

namespace ThingSet {

/// @brief Base class for streaming encoders.
/// @tparam Size The size of the chunks to write.
template <size_t Size> class StreamingThingSetBinaryEncoder : public virtual ThingSetBinaryEncoder
{
protected:
    zcbor_state_t _state[BINARY_ENCODER_DEFAULT_MAX_DEPTH];
    std::array<uint8_t, Size * 2> _buffer;
    size_t _exportedLength;

public:
    StreamingThingSetBinaryEncoder() : _exportedLength(0)
    {}

    size_t getEncodedLength() const override
    {
        return _exportedLength;
    }

    bool encodeKeysAsIds() const override
    {
        return true;
    }

    bool getIsForwardOnly() const override
    {
        return true;
    }

    template <typename T> bool encodePair(uint32_t id, T &value)
    {
        return encode(id) && encode(value);
    }

    /// @brief Write remaining buffer to the output.
    bool flush()
    {
        size_t headerSize = this->headerSize();
        size_t currentLength = _state->payload - &_buffer[headerSize];
        if (currentLength > (Size - headerSize)) {
            if (!writeIfNecessary()) {
                return false;
            }
            currentLength = _state->payload - &_buffer[headerSize];
        }
        size_t remainder = _buffer.size() - currentLength;
        memset(&_buffer[currentLength], 0, remainder);
        bool result = write(currentLength, true);
        _exportedLength += currentLength;
        return result;
    }

protected:
    virtual size_t headerSize() const
    {
        return 0;
    }

    bool ensureState() override
    {
        return writeIfNecessary();
    }

    zcbor_state_t *getState() override
    {
        return _state;
    }

    virtual bool write(size_t length, bool flushing) = 0;

private:
    /// @brief If the buffer contains more than @ref Size bytes worth of data,
    /// writes the data to the underlying stream.
    /// @return True if writing succeeded or was not necessary; false if writing failed.
    bool writeIfNecessary()
    {
        size_t headerSize = this->headerSize();
        size_t dataSize = Size - headerSize;
        size_t currentPos = _state->payload - &_buffer[headerSize];
        if (currentPos > dataSize) {
            if (!write(dataSize, false)) {
                // not sure what else to do with this for now
                return false;
            }
            memmove(&_buffer[headerSize], &_buffer[Size], Size); // move to start
            _exportedLength += dataSize; // keep track
            size_t newPos = currentPos - dataSize;
            zcbor_update_state(_state, &_buffer[headerSize + newPos], _buffer.size() - newPos);
        }
        return true;
    }
};

template <typename T, size_t Size>
concept StreamingBinaryEncoder = std::is_base_of_v<StreamingThingSetBinaryEncoder<Size>, T>;

} // namespace ThingSet
