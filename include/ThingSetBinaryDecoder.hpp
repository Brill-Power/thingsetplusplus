/*
 * Copyright (c) 2024 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */
#pragma once

#include "internal/bind_to_tuple.hpp"
#include "zcbor_decode.h"
#include <array>
#include <optional>
#include <tuple>

#define BINARY_ENCODER_MAX_NULL_TERMINATED_STRING_LENGTH 255
#define BINARY_DECODER_DEFAULT_MAX_DEPTH                 6

namespace ThingSet {

class ThingSetBinaryDecoder;

/// @brief Interface for values that can be decoded with a binary encoder.
class ThingSetBinaryDecodable
{
public:
    virtual bool decode(ThingSetBinaryDecoder &decoder) = 0;
};

/// @brief Binary protocol decoder for ThingSet.
class ThingSetBinaryDecoder
{
protected:
    virtual zcbor_state_t *getDecoder();

public:
    bool decode(float *value);
    bool decode(double *value);
    bool decode(bool *value);
    bool decode(uint8_t *value);
    bool decode(uint16_t *value);
    bool decode(uint32_t *value);
    bool decode(uint64_t *value);
    bool decode(int8_t *value);
    bool decode(int16_t *value);
    bool decode(int32_t *value);
    bool decode(int64_t *value);

    template <typename T, size_t size> bool decode(std::array<T, size> *value)
    {
        if (!zcbor_list_start_decode(getDecoder())) {
            return false;
        }

        if (size != getDecoder()->elem_count) {
            return false;
        }

        for (size_t i = 0; i < size; i++) {
            T *element = &(*value)[i];
            if (!decode(element)) {
                return false;
            }
        }

        return zcbor_list_end_decode(getDecoder());
    }

    template <typename T> bool decode(T *value)
    {
        auto bound = internal::bind_to_tuple(*value, [](auto &x) { return std::addressof(x); });
        if (!zcbor_map_start_decode(getDecoder())) {
            return false;
        }
        uint32_t id;
        while (decode(&id) && id < UINT16_MAX) {
            std::optional<ThingSetBinaryDecodable *> decodable = compile_switch(id, bound);
            if (!decodable.has_value()) {
                return false;
            }
            if (!decodable.value()->decode(*this)) {
                return false;
            }
        }
        return zcbor_map_end_decode(getDecoder());
    }

private:
    template <class Fields, std::size_t... Is>
    static std::optional<ThingSetBinaryDecodable *> compile_switch(uint32_t id, Fields fields,
                                                                   std::index_sequence<Is...>)
    {
        std::optional<ThingSetBinaryDecodable *> ret;
        std::initializer_list<std::optional<ThingSetBinaryDecodable *>>(
            { ((id == std::remove_pointer_t<std::remove_cvref_t<typename std::tuple_element<Is, Fields>::type>>::_id)
               ? ret = std::make_optional((std::get<Is>(fields))),
               std::nullopt : std::nullopt)... });
        return ret;
    }

    template <class Fields> static std::optional<ThingSetBinaryDecodable *> compile_switch(uint32_t id, Fields fields)
    {
        return compile_switch(id, fields, std::make_index_sequence<std::tuple_size_v<Fields>>());
    }
};

template <int size, int depth = BINARY_DECODER_DEFAULT_MAX_DEPTH>
class FixedSizeThingSetBinaryDecoder : public ThingSetBinaryDecoder
{
    friend class ThingSetBinaryDecoder;

private:
    // The start of the buffer
    const uint8_t *_buffer;
    zcbor_state_t _encoder[depth];

protected:
    zcbor_state_t *getDecoder() override
    {
        return _encoder;
    }

public:
    FixedSizeThingSetBinaryDecoder(uint8_t buffer[size]) : _buffer(buffer)
    {
        zcbor_new_decode_state(_encoder, depth, buffer, size, 1, NULL, 0);
    }
};

} // namespace ThingSet