/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "internal/bind_to_tuple.hpp"
#include <array>
#include <cstdint>
#include <list>
#include <map>
#include <string>
#include <string_view>
#include <tuple>

namespace ThingSet {

/// @brief Encoder base class for ThingSet.
class ThingSetEncoder
{
public:
    virtual size_t getEncodedLength() const = 0;

    virtual bool encode(const std::string_view &value) = 0;
    virtual bool encode(std::string_view &value) = 0;
    virtual bool encode(const std::string &value) = 0;
    virtual bool encode(std::string &value) = 0;
    virtual bool encode(const char *value) = 0;
    virtual bool encode(char *value) = 0;
    virtual bool encode(const float &value) = 0;
    virtual bool encode(float &value) = 0;
    virtual bool encode(float *value) = 0;
    virtual bool encode(const double &value) = 0;
    virtual bool encode(double &value) = 0;
    virtual bool encode(double *value) = 0;
    virtual bool encode(const bool &value) = 0;
    virtual bool encode(bool &value) = 0;
    virtual bool encode(const uint8_t &value) = 0;
    virtual bool encode(uint8_t &value) = 0;
    virtual bool encode(uint8_t *value) = 0;
    virtual bool encode(const uint16_t &value) = 0;
    virtual bool encode(uint16_t &value) = 0;
    virtual bool encode(uint16_t *value) = 0;
    virtual bool encode(const uint32_t &value) = 0;
    virtual bool encode(uint32_t &value) = 0;
    virtual bool encode(uint32_t *value) = 0;
    virtual bool encode(const uint64_t &value) = 0;
    virtual bool encode(uint64_t &value) = 0;
    virtual bool encode(uint64_t *value) = 0;
    virtual bool encode(const int8_t &value) = 0;
    virtual bool encode(int8_t &value) = 0;
    virtual bool encode(const int16_t &value) = 0;
    virtual bool encode(int16_t &value) = 0;
    virtual bool encode(const int32_t &value) = 0;
    virtual bool encode(int32_t &value) = 0;
    virtual bool encode(int32_t *value) = 0;
    virtual bool encode(const int64_t &value) = 0;
    virtual bool encode(int64_t &value) = 0;
    virtual bool encode(int64_t *value) = 0;
    virtual bool encodeNull() = 0;
    virtual bool encodePreamble() = 0;
    /// @brief Encode the start of a list. In forward-only encoding scenarios, you should
    /// use the overload which allows the number of elements in the list to be specified in
    /// advance.
    /// @return True if encoding succeeded, otherwise false.
    virtual bool encodeListStart() = 0;
    /// @brief Encode the start of a list, specifying the number of elements the list
    /// will contain.
    /// @param count The number of elements in the list.
    /// @return True if encoding succeeded, otherwise false.
    virtual bool encodeListStart(uint32_t count) = 0;
    virtual bool encodeListEnd() = 0;
    /// @brief Encode the end of a list, specifying the number of elements the list
    /// contained. It should match the number passed to @ref encodeListStart.
    /// @param count The number of elements in the list.
    /// @return True if encoding succeeded, otherwise false.
    virtual bool encodeListEnd(uint32_t count) = 0;
    virtual bool encodeMapStart() = 0;
    /// @brief Encode the start of a map, specifying the number of key-value pairs the map
    /// will contain.
    /// @param count The number of pairs in the map.
    /// @return True if encoding succeeded, otherwise false.
    virtual bool encodeMapStart(uint32_t count) = 0;
    virtual bool encodeMapEnd() = 0;
    /// @brief Encode the end of a map, specifying the number of key-value pairs the map
    /// contained. It should match the number passed to @ref encodeMapStart.
    /// @param count The number of pairs in the map.
    /// @return True if encoding succeeded, otherwise false.
    virtual bool encodeMapEnd(uint32_t count) = 0;

protected:
    template <typename TupleT, typename Fn> constexpr void for_each_element(TupleT &&tp, Fn &&fn)
    {
        std::apply([&fn]<typename... T>(T &&...args) { (fn(std::forward<T>(args)), ...); }, std::forward<TupleT>(tp));
    }
};

template <typename Derived>
class ThingSetEncoderExtensions : public ThingSetEncoder
{
public:
    using ThingSetEncoder::encode;
    /// @brief Encode a linked list.
    /// @tparam T The type of items in the list.
    /// @param value A reference to the list to be encoded.
    /// @return True if encoding succeeded, otherwise false.
    template <typename T> bool encode(std::list<T> &value)
    {
        if (!encodeListStart(value.size())) {
            return false;
        }
        for (T &item : value) {
            if (!encode(item)) {
                return false;
            }
        }
        return encodeListEnd(value.size());
    }

    /// @brief Encode a map.
    /// @tparam K The type of the keys in the map.
    /// @tparam V The type of the values in the map.
    /// @param map A reference to the map to be encoded.
    /// @return True if encoding succeeded, otherwise false.
    template <typename K, typename V> bool encode(std::map<K, V> &map)
    {
        if (!encodeMapStart(map.size())) {
            return false;
        }
        for (auto const &[key, value] : map) {
            std::pair<K, V> valuePair(key, value);
            if (!static_cast<Derived *>(this)->encode(valuePair)) {
                return false;
            }
        }
        return encodeMapEnd(map.size());
    }

    /// @brief Encode an object (structure or class) as a map.
    /// @tparam T The type to encode.
    /// @param value A reference to the value to be encoded.
    /// @return True if encoding succeeded, otherwise false.
    template <typename T>
        requires std::is_class_v<T>
    bool encode(T &value)
    {
        auto bound = internal::bind_to_tuple(value, [](auto &x) { return std::addressof(x); });
        auto count = std::tuple_size_v<decltype(bound)>;
        if (!encodeMapStart(count)) {
            return false;
        }
        for_each_element(bound, [this](auto &prop) {
            auto pair = std::make_pair(prop->getId(), prop->getValue());
            static_cast<Derived *>(this)->encode(pair);
        });
        return encodeMapEnd(count);
    }

    /// @brief Encode an array as a list.
    /// @tparam T The type of elements in the array.
    /// @tparam size The size of the array.
    /// @param value A reference to the array to be encoded.
    /// @return True if encoding succeeded, otherwise false.
    template <typename T, size_t size> bool encode(std::array<T, size> &value)
    {
        return static_cast<Derived *>(this)->encode(value.data(), value.size());
    }

    template <typename T, size_t size> bool encode(T value[size])
    {
        return static_cast<Derived *>(this)->encode(value, size);
    }

protected:
    template <typename TupleT, typename Fn> constexpr void for_each_element(TupleT &&tp, Fn &&fn)
    {
        std::apply([&fn]<typename... T>(T &&...args) { (fn(std::forward<T>(args)), ...); }, std::forward<TupleT>(tp));
    }
};

}; // namespace ThingSet
