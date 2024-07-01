/*
 * Copyright (c) 2024 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */
#pragma once

#include "internal/bind_to_tuple.hpp"
#include "zcbor_encode.h"
#include <array>
#include <list>
#include <map>
#include <tuple>

#define BINARY_ENCODER_MAX_NULL_TERMINATED_STRING_LENGTH 255
#define BINARY_ENCODER_DEFAULT_MAX_DEPTH                 6

namespace ThingSet {

/// @brief Binary protocol encoder for ThingSet.
class ThingSetBinaryEncoder
{
protected:
    virtual zcbor_state_t *getState() = 0;

public:
    virtual size_t getEncodedLength() = 0;

    bool encode(const std::string_view &value);
    bool encode(std::string_view &value);
    bool encode(const std::string &value);
    bool encode(std::string &value);
    bool encode(const char *value);
    bool encode(char *value);
    bool encode(const float &value);
    bool encode(float &value);
    bool encode(const double &value);
    bool encode(double &value);
    bool encode(const bool &value);
    bool encode(bool &value);
    bool encode(const uint8_t &value);
    bool encode(uint8_t &value);
    bool encode(const uint16_t &value);
    bool encode(uint16_t &value);
    bool encode(const uint32_t &value);
    bool encode(uint32_t &value);
    bool encode(const uint64_t &value);
    bool encode(uint64_t &value);
    bool encode(const int8_t &value);
    bool encode(int8_t &value);
    bool encode(const int16_t &value);
    bool encode(int16_t &value);
    bool encode(const int32_t &value);
    bool encode(int32_t &value);
    bool encode(const int64_t &value);
    bool encode(int64_t &value);
    bool encodeNull();
    bool encodeListStart();
    bool encodeListEnd();
    bool encodeMapStart();
    bool encodeMapEnd();

    template <typename K, typename V> bool encode(std::pair<K, V> &pair)
    {
        return encode(pair.first) && encode(pair.second);
    }

    template <typename T> bool encode(std::list<T> &value)
    {
        if (!zcbor_list_start_encode(getState(), value.size())) {
            return false;
        }
        for (T &item : value) {
            if (!encode(item)) {
                return false;
            }
        }
        return zcbor_list_end_encode(getState(), value.size());
    }

    template <typename K, typename V> bool encode(std::map<K, V> &map)
    {
        if (!zcbor_map_start_encode(getState(), map.size())) {
            return false;
        }
        for (auto const &[key, value] : map) {
            if (!encode(key) || !encode(value)) {
                return false;
            }
        }
        return zcbor_map_end_encode(getState(), map.size());
    }

    template <typename T> bool encode(T &value)
    {
        auto bound = internal::bind_to_tuple(value, [](auto &x) { return std::addressof(x); });
        auto items = std::tuple_size_v<decltype(bound)>;
        if (!zcbor_map_start_encode(getState(), items)) {
            return false;
        }
        for_each_element(bound, [this](auto &prop) {
            auto id = prop->getId();
            encode(id);
            prop->encode(*this);
        });
        return zcbor_map_end_encode(getState(), items);
    }

    template <typename T, size_t size> bool encode(std::array<T, size> &value)
    {
        return this->encode(value.data(), value.size());
    }

    template <typename T, size_t size> bool encode(T value[size])
    {
        return this->encode(value, size);
    }

    template <typename... TArgs> bool encodeList(TArgs... args)
    {
        const size_t count = sizeof...(TArgs);
        return zcbor_list_start_encode(getState(), count) && encode_and_shift(this, args...)
               && zcbor_list_end_encode(getState(), count);
    }

private:
    template <typename T> bool encode(T *value, size_t size)
    {
        bool result = zcbor_list_start_encode(getState(), size);
        for (size_t i = 0; i < size; i++) {
            result &= this->encode(value[i]);
        }
        return result && zcbor_list_end_encode(getState(), size);
    }

    inline bool encode_and_shift(ThingSetBinaryEncoder *encoder)
    {
        return true;
    }

    template <typename T, typename... TArgs> bool encode_and_shift(ThingSetBinaryEncoder *encoder, T arg, TArgs... rest)
    {
        return encoder->encode(arg) && encode_and_shift(encoder, rest...);
    }

    template <typename TupleT, typename Fn> constexpr void for_each_element(TupleT &&tp, Fn &&fn)
    {
        std::apply([&fn]<typename... T>(T &&...args) { (fn(std::forward<T>(args)), ...); }, std::forward<TupleT>(tp));
    }
};

template <int depth = BINARY_ENCODER_DEFAULT_MAX_DEPTH>
class FixedSizeThingSetBinaryEncoder : public ThingSetBinaryEncoder
{
private:
    // The start of the buffer
    const uint8_t *_buffer;
    zcbor_state_t _state[depth];

protected:
    zcbor_state_t *getState() override
    {
        return _state;
    }

public:
    FixedSizeThingSetBinaryEncoder(uint8_t *buffer, size_t size) : _buffer(buffer)
    {
        zcbor_new_encode_state(_state, depth, buffer, size, 1);
    }

    size_t getEncodedLength() override
    {
        return _state->payload - _buffer;
    }
};

/// @brief Interface for values that can be encoded with a binary encoder.
class ThingSetBinaryEncodable
{
public:
    virtual bool encode(ThingSetBinaryEncoder &encoder) = 0;
};

}; // namespace ThingSet
