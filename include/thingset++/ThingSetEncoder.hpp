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
#include <typeinfo>

namespace ThingSet {

class ThingSetEncodable;

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
    bool encode(const ThingSetEncodable &value);
    bool encode(ThingSetEncodable &value);
    bool encode(ThingSetEncodable *value);
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
    virtual bool encodeListStart(const uint32_t &count) = 0;
    virtual bool encodeListEnd() = 0;
    /// @brief Encode the end of a list, specifying the number of elements the list
    /// contained. It should match the number passed to @ref encodeListStart.
    /// @param count The number of elements in the list.
    /// @return True if encoding succeeded, otherwise false.
    virtual bool encodeListEnd(const uint32_t &count) = 0;
    virtual bool encodeMapStart() = 0;
    /// @brief Encode the start of a map, specifying the number of key-value pairs the map
    /// will contain.
    /// @param count The number of pairs in the map.
    /// @return True if encoding succeeded, otherwise false.
    virtual bool encodeMapStart(const uint32_t &count) = 0;
    virtual bool encodeMapEnd() = 0;
    /// @brief Encode the end of a map, specifying the number of key-value pairs the map
    /// contained. It should match the number passed to @ref encodeMapStart.
    /// @param count The number of pairs in the map.
    /// @return True if encoding succeeded, otherwise false.
    virtual bool encodeMapEnd(const uint32_t &count) = 0;

    /// @brief Encode a linked list.
    /// @tparam T The type of items in the list.
    /// @param value A reference to the list to be encoded.
    /// @return True if encoding succeeded, otherwise false.
    template <typename T> bool encode(const std::list<T> &value)
    {
        if (!encodeListStart(value.size())) {
            return false;
        }
        for (const T &item : value) {
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
    template <typename K, typename V> bool encode(const std::map<K, V> &map)
    {
        if (!encodeMapStart(map.size())) {
            return false;
        }
        for (const std::pair<K, V>& pair : map) {
            if (!encode(pair)) {
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
    bool encode(const T &value)
    {
        auto bound = internal::bind_to_tuple(value, [](auto &x) { return std::addressof(x); });
        auto count = std::tuple_size_v<decltype(bound)>;
        if (!encodeMapStart(count)) {
            return false;
        }
        for_each_element(bound, [this]<typename P>(P &prop) {
            if constexpr (std::is_convertible_v<P, const ThingSetEncodable *>) {
                if (this->encodeKeysAsIds()) {
                    this->encode(prop->getId());
                } else {
                    this->encode(prop->getName());
                }
                this->encodeKeyValuePairSeparator();
                auto value = prop->getValue();
                this->encode(value);
                this->encodeListSeparator();
            }
        });
        return encodeMapEnd(count);
    }

    /// @brief Encode an array as a list.
    /// @tparam T The type of elements in the array.
    /// @tparam size The size of the array.
    /// @param value A reference to the array to be encoded.
    /// @return True if encoding succeeded, otherwise false.
    template <typename T, size_t size> bool encode(const std::array<T, size> &value)
    {
        return this->encode(value.data(), value.size());
    }

    template <typename T, size_t size> bool encode(T value[size])
    {
        return this->encode(value, size);
    }

    /// @brief Encode a pair. This is probably most useful inside a map.
    /// @tparam K The type of the first value in the pair.
    /// @tparam V The type of the second value in the pair.
    /// @param pair A reference to the pair to be encoded.
    /// @return True if encoding succeeded, otherwise false.
    template <typename K, typename V> bool encode(const std::pair<K, V> &pair)
    {
        return encode(pair.first) && encodeKeyValuePairSeparator() && encode(pair.second) && encodeListSeparator();
    }

    template <typename T> bool encode(const T *value, const size_t &size)
    {
        bool result = encodeListStart(size);
        for (size_t i = 0; i < size; i++) {
            result &= this->encode(value[i]) && this->encodeListSeparator();
        }
        return result && encodeListEnd(size);
    }

    template <typename... TArgs> bool encodeList(TArgs... args)
    {
        const size_t count = sizeof...(TArgs);
        return encodeListStart(count) && encodeAndShift(args...) && encodeListEnd(count);
    }

protected:
    virtual bool encodeListSeparator() = 0;
    virtual bool encodeKeyValuePairSeparator() = 0;
    virtual bool encodeKeysAsIds() const = 0;

private:
    inline bool encodeAndShift()
    {
        return true;
    }

    template <typename T, typename... TArgs> bool encodeAndShift(T arg, TArgs... rest)
    {
        return encode(arg) && encodeListSeparator() && encodeAndShift(rest...);
    }

    template <typename TupleT, typename Fn> constexpr void for_each_element(TupleT &&tp, Fn &&fn)
    {
        std::apply([&fn]<typename... T>(T &&...args) { (fn(std::forward<T>(args)), ...); }, std::forward<TupleT>(tp));
    }
};

/// @brief Interface for values that can be encoded with any encoder.
class ThingSetEncodable
{
public:
    virtual bool encode(ThingSetEncoder &encoder) const = 0;
};

}; // namespace ThingSet
