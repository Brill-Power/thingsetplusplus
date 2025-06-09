/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "internal/bind_to_tuple.hpp"
#include "zcbor_decode.h"
#include <array>
#include <functional>
#include <map>
#include <optional>
#include <string>
#include <tuple>

#define TEXT_ENCODER_BUFFER_SIZE 1024

namespace ThingSet {

class ThingSetTextDecoder;

/// @brief Options to control the behaviour of the decoder.
enum ThingSetTextDecoderOptions
{
    /// @brief If set, permits the decoding of arrays which are smaller than the declared
    /// size of the destination array.
    // allowUndersizedArrays = 1 << 0, // todo readd this
};

/// @brief Interface for values that can be decoded with a text encoder.
class ThingSetTextDecodable
{
public:
    virtual bool decode(ThingSetTextDecoder &decoder) = 0;
};

/// @brief Text protocol decoder for ThingSet.
class ThingSetTextDecoder
{
private:
    char *_inputBuffer;
    size_t _bufferSize;
    size_t _bufferElemPtr;
    ThingSetTextDecoderOptions _options;

protected:
    /// @brief Gets whether the stream that this decoder is decoding
    /// is forward-only.
    /// @return True if forward-only, otherwise false.
    virtual bool getIsForwardOnly() const;

public:
    ThingSetTextDecoder(char *buffer, size_t size) : _inputBuffer(buffer), _bufferSize(size), _bufferElemPtr(0)
    {}
    ThingSetTextDecoder(char *buffer, size_t size, ThingSetTextDecoderOptions options)
        : _inputBuffer(buffer), _bufferSize(size), _bufferElemPtr(0), _options(options)
    {}

    bool decode(std::string *value);
    bool decode(char *value);
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
    bool decodeNull();
    /// @brief Decodes a list by iterating over its elements and invoking a callback
    /// with the index of each element. The callback should decode the element.
    /// @param callback The callback to be invoked for each element. The parameter is the
    /// index of the current element. The callback should decode the value and return true
    /// if successful, otherwise false.
    /// @return True if decoding succeeded, otherwise false.
    bool decodeList(std::function<bool(size_t)> callback);
    bool decodeListStart();
    bool decodeListEnd();
    bool decodeMapStart();
    bool decodeMapEnd();
    bool decodeListSeparator(); // todo make this private?
    bool decodeMapSeparator();  // todo make this private?

    /// @brief Decode a list into a tuple.
    /// @tparam ...Args The types of the elements in the list.
    /// @param tuple The tuple into which the list elements should be decoded.
    /// @return True if decoding succeeded, otherwise false.
    template <typename... Args> bool decodeList(std::tuple<Args...> &tuple)
    {
        bool result = decodeListStart();
        if (result) {
            std::apply([&](auto &&...args) { ((result &= decode(&args), decodeListSeparator()), ...); }, tuple);
        }
        return result & decodeListEnd();
    }

    /// @brief Decodes a map by iterating over its keys and invoking a callback for each key.
    /// @tparam K The type of the keys in the map.
    /// @param callback The callback to be invoked each time a key is decoded. This callback should decode the value and
    /// return true, or fail and return false.
    /// @return True if decoding succeeded, otherwise false.
    template <typename K> bool decodeMap(std::function<bool(K &)> callback)
    {
        if (!decodeMapStart()) {
            return false;
        }

        while ((_bufferElemPtr < _bufferSize) && (_inputBuffer[_bufferElemPtr] != '}')) {
            K key;
            if (!decode(&key)) {
                return false;
            }
            _bufferElemPtr++; // skip :
            if (!callback(key)) {
                return false;
            }
            decodeMapSeparator();
        }
        return decodeMapEnd();
    }

    /// @brief Decode a list into an array.
    /// @tparam T The type of items in the array.
    /// @tparam size The length of the array.
    /// @param value A pointer to the array into which list elements should be decoded.
    /// @return True if decoding succeeded, otherwise false.
    template <typename T, size_t size> bool decode(std::array<T, size> *value)
    {
        return decode(value->data(), value->size());
    }

    /// @brief Decode a list into an array of the specified length.
    /// @tparam T The type of items in the array.
    /// @param value A pointer to the start of the array into which list elements should be decoded.
    /// @param size The length of the array.
    /// @return True if decoding succeeded, otherwise false.
    template <typename T> bool decode(T *value, size_t size)
    {
        if (!decodeListStart()) {
            return false;
        }

        for (size_t i = 0; i < size; i++) {
            T *element = &value[i];
            if (!decode(element)) {
                return false;
            }
            decodeListSeparator();
        }

        return decodeListEnd();
    }

    /// @brief Decode a map into a pointer to a class or structure.
    /// @tparam T The type of the class or structure to be decoded.
    /// @param value A pointer to the class or structure.
    /// @return True if decoding succeeded, otherwise false.
    template <typename T>
        requires std::is_class_v<T>
    bool decode(T *value)
    {
        printf("in decode func"); // todo delete
        auto bound = internal::bind_to_tuple(*value, [](auto &x) { return std::addressof(x); });
        if (!decodeMapStart()) {
            return false;
        }
        std::string name;
        while (decode(&name)) {
            _bufferElemPtr++; // ignore the ':'
            if (!switchDecode(name, bound)) {
                return false;
            }
            _bufferElemPtr++; // ignore the ','
        }
        return decodeMapEnd();
    }

private: // todo repeated label, same in binarydecoder
    // adapted from https://stackoverflow.com/questions/46278997/variadic-templates-and-switch-statement
    template <class Fields, std::size_t... Is>
    bool switchDecode(const std::string &name, Fields &f, std::index_sequence<Is...>)
    {
        bool ret = false;
        return ([&] {
            if (name == std::remove_pointer_t<std::remove_cvref_t<typename std::tuple_element<Is, Fields>::type>>::name) {
                ret = std::get<Is>(f)->decode(*this);
                return true;
            }
            return false;
        }() || ...);
        return ret;
    }

    template <class Fields> bool switchDecode(const std::string &name, Fields &f)
    {
        return switchDecode<Fields>(name, f, std::make_index_sequence<std::tuple_size_v<Fields>>());
    }

    template <typename T> bool decodeValue(T *value, T (*parseFunc)(const char *, char **))
    {
        const char *startPtr = &_inputBuffer[_bufferElemPtr];
        char *endPtr;
        *value = parseFunc(startPtr, &endPtr);
        size_t consumed = endPtr - startPtr;
        _bufferElemPtr += consumed;
        return true;
    }
};

} // namespace ThingSet