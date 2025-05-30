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

}; // namespace ThingSet
