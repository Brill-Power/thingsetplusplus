/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <thingset++/ThingSet.hpp>

template <size_t Capacity>
class Buffer : public ThingSet::ThingSetDecodable, public ThingSet::ThingSetEncodable
{
private:
    std::array<uint8_t, Capacity> _buffer;
    size_t _size;

public:
    Buffer() : _buffer(), _size(0)
    {}

    std::array<uint8_t, Capacity> &buffer()
    {
        return _buffer;
    }

    size_t &size()
    {
        return _size;
    }

    const size_t &size() const
    {
        return _size;
    }

    bool decode(ThingSet::ThingSetDecoder &decoder) override
    {
        return decoder.decodeBytes(_buffer, _size);
    }

    bool encode(ThingSet::ThingSetEncoder &encoder) const override
    {
        return encoder.encodeBytes(_buffer, _size);
    }
};