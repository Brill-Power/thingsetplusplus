/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "ThingSetEncoder.hpp"
#include <cstring>

namespace ThingSet {

/// @brief Text protocol encoder for ThingSet.
class ThingSetTextEncoder : public ThingSetEncoder
{
public:
    // todo move these to be private?
    char _rsp[ENCODER_MAX_NULL_TERMINATED_STRING_LENGTH]; // todo change size here? or just change to uint8_t*
    size_t _rsp_size;
    size_t _rsp_pos;
    uint8_t _depth;

    template <size_t Size>
    ThingSetTextEncoder(std::array<uint8_t, Size> buffer) : ThingSetTextEncoder(buffer.data(), buffer.size())
    {}
    ThingSetTextEncoder(char (&buffer)[ENCODER_MAX_NULL_TERMINATED_STRING_LENGTH], size_t size)
    {
        memcpy(_rsp, buffer, ENCODER_MAX_NULL_TERMINATED_STRING_LENGTH);
        _rsp_size = size;
        _rsp_pos = 0;
        _depth = 0;
    }

    size_t getEncodedLength() const override
    {
        return _rsp_pos;
    }

    template <typename T> inline bool addResponseValue(T value, const char *format)
    { // todo make this private?

        // todo move this to a getFutureEncodedLength in header file? make this private too?
        int bytes_required = snprintf(nullptr, 0, format, value); // determine the length of value as a string

        if (sizeof(_rsp) < _rsp_pos + bytes_required) {
            return false;
        }

        // todo use getEncodedLength instead of sizeof section here?
        int bytes_written = snprintf(_rsp + _rsp_pos, sizeof(_rsp) - _rsp_pos, format, value);
        _rsp_pos += bytes_written;

        return true;
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
    bool encode(bool *value) override;
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
    bool encode(int8_t *value) override;
    bool encode(const int16_t &value) override;
    bool encode(int16_t &value) override;
    bool encode(int16_t *value) override;
    bool encode(const int32_t &value) override;
    bool encode(int32_t &value) override;
    bool encode(int32_t *value) override;
    bool encode(const int64_t &value) override;
    bool encode(int64_t &value) override;
    bool encode(int64_t *value) override;
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

    // todo override this from base class instead of duplicating

    /// @brief Encode a pair. This is probably most useful inside a map.
    /// @tparam K The type of the first value in the pair.
    /// @tparam V The type of the second value in the pair.
    /// @param pair A reference to the pair to be encoded.
    /// @return True if encoding succeeded, otherwise false.
    template <typename K, typename V> bool encode(std::pair<K, V> &pair)
    {
        return encode(pair.first) && addResponseValue(":", "%s") && encode(pair.second) && addResponseValue(",", "%s");
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
            // todo override this from base class instead of duplicating and call std::pair encode here instead of
            // duplicating?
            if (!encode(key) || !addResponseValue(":", "%s") || !encode(value) || !addResponseValue(",", "%s")) {
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
            addResponseValue(":", "%s");
            prop->encode(*this);
            addResponseValue(",", "%s");
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

    // todo override this from base class instead of duplicating
    template <typename T> bool encode(T *value, size_t size)
    {
        bool result = encodeListStart(size);
        for (size_t i = 0; i < size; i++) {
            result &= this->encode(value[i]);
            addResponseValue(",", "%s");
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
        return encode(arg) && addResponseValue(",", "%s") && encodeAndShift(rest...);
    }

    template <typename TupleT, typename Fn> constexpr void for_each_element(TupleT &&tp, Fn &&fn)
    {
        std::apply([&fn]<typename... T>(T &&...args) { (fn(std::forward<T>(args)), ...); }, std::forward<TupleT>(tp));
    }
};

/// @brief Interface for values that can be encoded with a text encoder.
class ThingSetTextEncodable
{
public:
    virtual bool encode(ThingSetTextEncoder &encoder) = 0;
};

}; // namespace ThingSet
