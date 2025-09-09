/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <cstdint>
#include <string>
#include <functional>
#include <optional>
#include "internal/bind_to_tuple.hpp"

namespace ThingSet {

enum struct ThingSetEncodedNodeType : uint8_t {
    unknown = 0,
    primitive = 1,
    string = 2,
    list = 3,
    map = 4,
};

class ThingSetDecodable;
class ThingSetNode;

template<typename T>
concept IsDecodable = std::is_base_of_v<ThingSetDecodable, T>;

/// @brief Base class for decoders.
class ThingSetDecoder
{
public:
    virtual bool decode(std::string *value) = 0;
    /// @todo Figure out how to do bounds checking on this.
    virtual bool decode(char *value) = 0;
    virtual bool decode(float *value) = 0;
    virtual bool decode(double *value) = 0;
    virtual bool decode(bool *value) = 0;
    virtual bool decode(uint8_t *value) = 0;
    virtual bool decode(uint16_t *value) = 0;
    virtual bool decode(uint32_t *value) = 0;
    virtual bool decode(uint64_t *value) = 0;
    virtual bool decode(int8_t *value) = 0;
    virtual bool decode(int16_t *value) = 0;
    virtual bool decode(int32_t *value) = 0;
    virtual bool decode(int64_t *value) = 0;
    virtual bool decodeNull() = 0;
    /// @brief Decodes a list by iterating over its elements and invoking a callback
    /// with the index of each element. The callback should decode the element.
    /// @param callback The callback to be invoked for each element. The parameter is the
    /// index of the current element. The callback should decode the value and return true
    /// if successful, otherwise false.
    /// @return True if decoding succeeded, otherwise false.
    bool decodeList(std::function<bool(size_t)> callback);
    virtual bool decodeListStart() = 0;
    virtual bool decodeListEnd() = 0;

    virtual bool decodeBytes(uint8_t *buffer, size_t capacity, size_t &size) = 0;

    template <size_t Capacity>
    bool decodeBytes(std::array<uint8_t, Capacity> &buffer, size_t &size)
    {
        return decodeBytes(buffer.data(), Capacity, size);
    }

    virtual ThingSetEncodedNodeType peekType() = 0;

    virtual bool skip() = 0;
    bool skipUntil(ThingSetEncodedNodeType sought);

    template <typename T, typename U = std::underlying_type_t<T>> requires std::is_enum_v<T>
    bool decode(T *value)
    {
        U underlying;
        if (decode(&underlying)) {
            *value = static_cast<T>(underlying);
            return true;
        }

        return false;
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

        size_t elementCount;
        if (!ensureListSize(size, elementCount)) {
            return false;
        }

        for (size_t i = 0; i < elementCount; i++) {
            T *element = &value[i];
            if (!decode(element)) {
                return false;
            }
        }

        return decodeListEnd();
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

    /// @brief Decodes into a pointer to a @see ThingSetDecodable value.
    /// @tparam T The type of the value to be decoded.
    /// @param value A pointer to the value to be decoded into.
    /// @return True if decoding succeeded, otherwise false.
    template <IsDecodable T>
    bool decode(T *value)
    {
        return value->decode(*this);
    }

    /// @brief Decode a map into a pointer to a class or structure.
    /// @tparam T The type of the class or structure to be decoded.
    /// @param value A pointer to the class or structure.
    /// @return True if decoding succeeded, otherwise false.
    template <typename T>
        requires std::is_class_v<T> && (!IsDecodable<T>)
    bool decode(T *value)
    {
        auto bound = internal::bind_to_tuple(*value, [](auto &x) { return std::addressof(x); });
        if (!decodeMapStart()) {
            return false;
        }
        std::optional<uint32_t> id;
        std::optional<std::string> name;
        while (decodeKey(id, name)) {
            if (id.has_value()) {
                if (!switchDecode(id.value(), []<typename P>(){ return P::id; }, bound)) {
                    if (!skip()) {
                        return false;
                    }
                }
            } else if (name.has_value()) {
                if (!switchDecode(name.value(), []<typename P>(){ return P::name; }, bound)) {
                    if (!skip()) {
                        return false;
                    }
                }
            } else {
                return false;
            }
        }
        return decodeMapEnd();
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

        while (isInMap()) {
            K key;
            if (!decode(&key)) {
                return false;
            }
            if (!callback(key)) {
                return false;
            }
        }
        return decodeMapEnd();
    }

    /// @brief Decode a list into a tuple.
    /// @tparam ...Args The types of the elements in the list.
    /// @param tuple The tuple into which the list elements should be decoded.
    /// @return True if decoding succeeded, otherwise false.
    template <typename... Args> bool decodeList(std::tuple<Args...> &tuple)
    {
        bool result = decodeListStart();
        if (result) {
            std::apply([&](auto &&...args) { ((result &= decode(&args)), ...); }, tuple);
        }
        return result & decodeListEnd();
    }

protected:
    virtual bool decodeMapStart() = 0;
    virtual bool decodeMapEnd() = 0;
    virtual bool isInMap() const = 0;
    virtual bool isInList() const = 0;
    virtual bool ensureListSize(const size_t size, size_t &elementCount) = 0;

private:
    bool decodeKey(std::optional<uint32_t> &id, std::optional<std::string> &name);

    // thanks to https://stackoverflow.com/questions/75163129/convert-variadic-template-ints-to-switch-statement?rq=3
    // adapted from https://stackoverflow.com/questions/46278997/variadic-templates-and-switch-statement
    template <typename Key, class Fields, std::size_t... Is>
    bool switchDecode(const Key &key, auto keySelector, Fields &f, std::index_sequence<Is...>)
    {
        bool ret = false;
        return ([&] {
            // check if this is actually a property; it might not be, in which case just skip it
            using propertyType = typename std::remove_pointer_t<std::remove_cvref_t<typename std::tuple_element<Is, Fields>::type>>;
            if constexpr (std::is_convertible_v<propertyType *, ThingSetDecodable *> and
                std::is_convertible_v<propertyType *, ThingSetNode *>) {
                // https://lemire.me/blog/2025/03/15/speeding-up-c-code-with-template-lambdas/
                // (for helping with passing template parameters to lambdas)
                if (key == keySelector.template operator()<propertyType>()) {
                    ret = std::get<Is>(f)->decode(*this);
                    return true;
                }
            }
            return false;
        }() || ...);
        return ret;
    }

    template <typename Key, class Fields> bool switchDecode(const Key &key, auto keySelector, Fields &f)
    {
        return switchDecode<Key, Fields>(key, keySelector, f, std::make_index_sequence<std::tuple_size_v<Fields>>());
    }
};

/// @brief Interface for values that can be decoded with any decoder.
class ThingSetDecodable
{
public:
    virtual bool decode(ThingSetDecoder &decoder) = 0;
};

} // namespace ThingSet