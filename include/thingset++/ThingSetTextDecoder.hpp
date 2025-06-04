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
    allowUndersizedArrays = 1 << 0,
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
    bool decodeListSeparator(); // todo make this private?

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
    // template <typename K> bool decodeMap(std::function<bool(K &)> callback)
    // {
    //     todo implement this if (!zcbor_map_start_decode(getState()))
    //     {
    //         return false;
    //     }

    //     while (_bufferElemPtr < _bufferSize) {
    //         K key;
    //         if (!decode(&key)) {
    //             return false;
    //         }
    //         if (!callback(key)) {
    //             return false;
    //         }
    //     }

    //     return zcbor_map_end_decode(getState());
    //     return true;
    // }

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
        size_t elementCount;
        return decode(value, size, elementCount);
    }

    /// @brief Decode a map into a pointer to a class or structure.
    /// @tparam T The type of the class or structure to be decoded.
    /// @param value A pointer to the class or structure.
    /// @return True if decoding succeeded, otherwise false.
    template <typename T>
        requires std::is_class_v<T>
    bool decode(T *value)
    {
        // todo implement this
        // auto bound = internal::bind_to_tuple(*value, [](auto &x) { return std::addressof(x); });
        // if (!zcbor_map_start_decode(getState())) {
        //     return false;
        // }
        // uint32_t id;
        // while (decode(&id) && id < UINT16_MAX) {
        //     std::function<bool(ThingSetTextDecoder &, decltype(bound) &)> func = compile_switch(id, bound);
        //     if (!func(*this, bound)) {
        //         return false;
        //     }
        // }
        // return zcbor_map_end_decode(getState());
        return true;
    }

protected: // todo repeated label, same in binary decoder
    template <typename T> bool decode(T *value, size_t size, size_t &elementCount)
    {
        // todo implement this
        // if (!zcbor_list_start_decode(getState())) {
        //     return false;
        // }

        // // check if number of elements in stream matches array size
        // elementCount = getState()->elem_count;
        // if (size > elementCount && (_options & ThingSetTextDecoderOptions::allowUndersizedArrays) == 0) {
        //     if (!getIsForwardOnly()) {
        //         // wind the state back to before we started parsing the list, so that a
        //         // call to `skip()` will skip the whole array
        //         zcbor_process_backup(getState(), ZCBOR_FLAG_RESTORE | ZCBOR_FLAG_CONSUME, ZCBOR_MAX_ELEM_COUNT);
        //         // because the backup is taken *after* it has consumed the byte(s) containing the number
        //         // of elements in the array, this restore doesn't really work
        //         // so we overwrite the payload pointer with the backup it takes
        //         getState()->payload = getState()->payload_bak;
        //         // we also need to increment this, because, yes, it has consumed this as well
        //         getState()->elem_count++;
        //     }
        //     return false;
        // }

        // for (size_t i = 0; i < elementCount; i++) {
        //     T *element = &value[i];
        //     if (!decode(element)) {
        //         return false;
        //     }
        // }

        // return zcbor_list_end_decode(getState());
        return true;
    }

private: // todo repeated label, same in binarydecoder
    // adapted from https://stackoverflow.com/questions/46278997/variadic-templates-and-switch-statement
    template <class Fields, std::size_t... Is>
    bool switchDecode(const uint32_t &id, Fields &f, std::index_sequence<Is...>)
    {
        bool ret = false;
        return ([&] {
            if (id == std::remove_pointer_t<std::remove_cvref_t<typename std::tuple_element<Is, Fields>::type>>::id) {
                std::get<Is>(f)->decode(*this);
                ret = std::get<Is>(f)->decode(*this);
                return true;
            }
            else {
                return false;
            }
        }() || ...);
        return ret;
    }

    template <class Fields>
    static std::function<bool(ThingSetTextDecoder &, Fields &)> switchDecode(uint32_t id, Fields)
    {
        return switchDecode<Fields>(id, std::make_index_sequence<std::tuple_size_v<Fields>>());
    }
};

} // namespace ThingSet