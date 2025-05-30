/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "ThingSetEncoder.hpp"
#include <cstring>

#define TEXT_ENCODER_BUFFER_SIZE 1024

namespace ThingSet {

/// @brief Text protocol encoder for ThingSet.
class ThingSetTextEncoder : public ThingSetEncoder
{
private:
    char *_responseBuffer;
    size_t _responseSize;
    size_t _responsePosition;
    uint8_t _depth;

    /// @brief Determine the length of the value as a string
    /// @return Number of characters in the string.
    template <typename T> inline int ensureBufferCapacity(T value, const char *format)
    {
        int bytesRequired = snprintf(nullptr, 0, format, value); // get size of value as a string

        if (_responseSize < _responsePosition + bytesRequired) {
            return false;
        }
        return true;
    }

    /// @brief Add a value to the response buffer as a string
    /// @return True if successful, false otherwise
    template <typename T> bool addResponseValue(T value, const char *format = "%s")
    {
        if (!ensureBufferCapacity(value, format)) {
            return false;
        }

        int bytesWritten =
            snprintf(_responseBuffer + getEncodedLength(), _responseSize - getEncodedLength(), format, value);
        _responsePosition += bytesWritten;

        return true;
    }

public:
    template <size_t Size>
    ThingSetTextEncoder(std::array<uint8_t, Size> buffer) : ThingSetTextEncoder(buffer.data(), buffer.size())
    {}
    ThingSetTextEncoder(char (&buffer)[TEXT_ENCODER_BUFFER_SIZE], size_t size)
        : _responseBuffer(buffer), _responseSize(size), _responsePosition(0), _depth(0)
    {}

    size_t getEncodedLength() const override
    {
        return _responsePosition;
    }

    bool encode(const std::string_view &value) override;
    bool encode(std::string_view &value) override;
    bool encode(const std::string &value) override;
    bool encode(std::string &value) override;
    bool encode(const char *value) override;
    bool encode(char *value) override;
    bool encode(const float &value) override;
    bool encode(float &value) override;
    bool encode(float *value) override;
    bool encode(const double &value) override;
    bool encode(double &value) override;
    bool encode(double *value) override;
    bool encode(const bool &value) override;
    bool encode(bool &value) override;
    bool encode(bool *value);
    bool encode(const uint8_t &value) override;
    bool encode(uint8_t &value) override;
    bool encode(uint8_t *value) override;
    bool encode(const uint16_t &value) override;
    bool encode(uint16_t &value) override;
    bool encode(uint16_t *value) override;
    bool encode(const uint32_t &value) override;
    bool encode(uint32_t &value) override;
    bool encode(uint32_t *value) override;
    bool encode(const uint64_t &value) override;
    bool encode(uint64_t &value) override;
    bool encode(uint64_t *value) override;
    bool encode(const int8_t &value) override;
    bool encode(int8_t &value) override;
    bool encode(int8_t *value);
    bool encode(const int16_t &value) override;
    bool encode(int16_t &value) override;
    bool encode(int16_t *value);
    bool encode(const int32_t &value) override;
    bool encode(int32_t &value) override;
    bool encode(int32_t *value) override;
    bool encode(const int64_t &value) override;
    bool encode(int64_t &value) override;
    bool encode(int64_t *value) override;
    bool encodeNull() override;
    bool encodePreamble() override;
    /// @brief Encode the start of a list. In forward-only encoding scenarios, you should
    /// use the overload which allows the number of elements in the list to be specified in
    /// advance.
    /// @return True if encoding succeeded, otherwise false.
    bool encodeListStart() override;
    /// @brief Encode the start of a list, specifying the number of elements the list
    /// will contain.
    /// @param count The number of elements in the list.
    /// @return True if encoding succeeded, otherwise false.
    bool encodeListStart(uint32_t count) override;
    bool encodeListEnd() override;
    /// @brief Encode the end of a list, specifying the number of elements the list
    /// contained. It should match the number passed to @ref encodeListStart.
    /// @param count The number of elements in the list.
    /// @return True if encoding succeeded, otherwise false.
    bool encodeListEnd(uint32_t count) override;
    bool encodeMapStart() override;
    /// @brief Encode the start of a map, specifying the number of key-value pairs the map
    /// will contain.
    /// @param count The number of pairs in the map.
    /// @return True if encoding succeeded, otherwise false.
    bool encodeMapStart(uint32_t count) override;
    bool encodeMapEnd() override;
    /// @brief Encode the end of a map, specifying the number of key-value pairs the map
    /// contained. It should match the number passed to @ref encodeMapStart.
    /// @param count The number of pairs in the map.
    /// @return True if encoding succeeded, otherwise false.
    bool encodeMapEnd(uint32_t count) override;

    /// @brief Encode a pair. This is probably most useful inside a map.
    /// @tparam K The type of the first value in the pair.
    /// @tparam V The type of the second value in the pair.
    /// @param pair A reference to the pair to be encoded.
    /// @return True if encoding succeeded, otherwise false.
    template <typename K, typename V> bool encode(std::pair<K, V> &pair)
    {
        return encode(pair.first) && addResponseValue(":") && encode(pair.second) && addResponseValue(",");
    }

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
            if (!encode(valuePair)) {
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
            auto id = prop->getName();
            encode(id);
            addResponseValue(":");
            prop->encode(*this);
            addResponseValue(",");
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
        return this->encode(value.data(), value.size());
    }

    template <typename T, size_t size> bool encode(T value[size])
    {
        return this->encode(value, size);
    }

    template <typename... TArgs> bool encodeList(TArgs... args)
    {
        const size_t count = sizeof...(TArgs);
        return encodeListStart(count) && encodeAndShift(args...) && encodeListEnd(count);
    }

    template <typename T> bool encode(T *value, size_t size)
    {
        bool result = encodeListStart(size);
        for (size_t i = 0; i < size; i++) {
            result &= this->encode(value[i]);
            addResponseValue(",");
        }
        return result && encodeListEnd(size);
    }

private:
    inline bool encodeAndShift()
    {
        return true;
    }

    template <typename T, typename... TArgs> bool encodeAndShift(T arg, TArgs... rest)
    {
        return encode(arg) && addResponseValue(",") && encodeAndShift(rest...);
    }
};

/// @brief Interface for values that can be encoded with a text encoder.
class ThingSetTextEncodable
{
public:
    virtual bool encode(ThingSetTextEncoder &encoder) = 0;
};

}; // namespace ThingSet
