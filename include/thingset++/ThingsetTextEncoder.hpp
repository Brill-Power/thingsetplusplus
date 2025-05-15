/*
 * Copyright (c) 2025 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */
#pragma once

#include "ThingsetEncoder.hpp"
#include "internal/bind_to_tuple.hpp"
#include "zcbor_encode.h"
#include <array>
#include <list>
#include <map>
#include <string>
#include <string_view>
#include <tuple>

#define TEXT_ENCODER_MAX_NULL_TERMINATED_STRING_LENGTH 255
#define TEXT_ENCODER_DEFAULT_MAX_DEPTH                 8 // todo delete this if encoder depth used in thingsetencoder

namespace ThingSet {

/// @brief Text protocol encoder for ThingSet.
class ThingSetTextEncoder : ThingSetEncoder
{
protected:
    virtual zcbor_state_t *getState() = 0;
    virtual bool getIsForwardOnly() const;

public:
    virtual size_t getEncodedLength() const = 0;

    // todo delete all of these and inherit from base
    bool encode(const std::string_view &value);
    bool encode(std::string_view &value);
    bool encode(const std::string &value);
    bool encode(std::string &value);
    bool encode(const char *value);
    bool encode(char *value);
    bool encode(const float &value);
    bool encode(float &value);
    bool encode(float *value);
    bool encode(const double &value);
    bool encode(double &value);
    bool encode(double *value);
    bool encode(const bool &value);
    bool encode(bool &value);
    bool encode(const uint8_t &value);
    bool encode(uint8_t &value);
    bool encode(uint8_t *value);
    bool encode(const uint16_t &value);
    bool encode(uint16_t &value);
    bool encode(uint16_t *value);
    bool encode(const uint32_t &value);
    bool encode(uint32_t &value);
    bool encode(uint32_t *value);
    bool encode(const uint64_t &value);
    bool encode(uint64_t &value);
    bool encode(uint64_t *value);
    bool encode(const int8_t &value);
    bool encode(int8_t &value);
    bool encode(const int16_t &value);
    bool encode(int16_t &value);
    bool encode(const int32_t &value);
    bool encode(int32_t &value);
    bool encode(int32_t *value);
    bool encode(const int64_t &value);
    bool encode(int64_t &value);
    bool encode(int64_t *value);
    bool encodeNull();
    /// @brief Encode the start of a list. In forward-only encoding scenarios, you should
    /// use the overload which allows the number of elements in the list to be specified in
    /// advance.
    /// @return True if encoding succeeded, otherwise false.
    bool encodeListStart();
    /// @brief Encode the start of a list, specifying the number of elements the list
    /// will contain.
    /// @param count The number of elements in the list.
    /// @return True if encoding succeeded, otherwise false.
    bool encodeListStart(uint32_t count);
    bool encodeListEnd();
    /// @brief Encode the end of a list, specifying the number of elements the list
    /// contained. It should match the number passed to @ref encodeListStart.
    /// @param count The number of elements in the list.
    /// @return True if encoding succeeded, otherwise false.
    bool encodeListEnd(uint32_t count);
    bool encodeMapStart();
    /// @brief Encode the start of a map, specifying the number of key-value pairs the map
    /// will contain.
    /// @param count The number of pairs in the map.
    /// @return True if encoding succeeded, otherwise false.
    bool encodeMapStart(uint32_t count);
    bool encodeMapEnd();
    /// @brief Encode the end of a map, specifying the number of key-value pairs the map
    /// contained. It should match the number passed to @ref encodeMapStart.
    /// @param count The number of pairs in the map.
    /// @return True if encoding succeeded, otherwise false.
    bool encodeMapEnd(uint32_t count);

    /// @brief Encode a pair. This is probably most useful inside a map.
    /// @tparam K The type of the first value in the pair.
    /// @tparam V The type of the second value in the pair.
    /// @param pair A reference to the pair to be encoded.
    /// @return True if encoding succeeded, otherwise false.
    template <typename K, typename V> bool encode(std::pair<K, V> &pair)
    {
        return encode(pair.first) && encode(pair.second);
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
            if (!encode(key) || !encode(value)) {
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
            auto id = prop->getId();
            encode(id);
            prop->encode(*this);
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
        return encode(arg) && encodeAndShift(rest...);
    }

    template <typename TupleT, typename Fn> constexpr void for_each_element(TupleT &&tp, Fn &&fn)
    {
        std::apply([&fn]<typename... T>(T &&...args) { (fn(std::forward<T>(args)), ...); }, std::forward<TupleT>(tp));
    }
};

template <int depth = TEXT_ENCODER_DEFAULT_MAX_DEPTH>
class FixedDepthThingSetTextEncoder : public virtual ThingSetTextEncoder
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
    FixedDepthThingSetTextEncoder(uint8_t *buffer, size_t size) : _buffer(buffer)
    {
        zcbor_new_encode_state(_state, depth, buffer, size, 1);
    }

    size_t getEncodedLength() const override
    {
        return _state->payload - _buffer;
    }
};

using DefaultFixedDepthThingSetTextEncoder = FixedDepthThingSetTextEncoder<TEXT_ENCODER_DEFAULT_MAX_DEPTH>;

/// @brief Interface for values that can be encoded with a text encoder.
class ThingSetTextEncodable
{
public:
    virtual bool encode(ThingSetTextEncoder &encoder) = 0;
};

}; // namespace ThingSet

/* todo move this---------------------------------------------------------------------------*/

/**
 * ThingSet context.
 *
 * Stores and handles all data objects exposed to different communication interfaces.
 */
struct thingset_context
{
    /**
     * Array of objects database provided during initialization
     */
    struct thingset_data_object *data_objects;

#ifdef CONFIG_THINGSET_OBJECT_LOOKUP_MAP
    /**
     * Array of linked lists: map for object ID lookup
     */
    sys_slist_t data_objects_lookup[CONFIG_THINGSET_OBJECT_LOOKUP_BUCKETS];
#endif

    /**
     * Number of objects in the data_objects array
     */
    size_t num_objects;

    /**
     * Semaphore to lock this context and avoid race conditions if the context may be used by
     * multiple threads in parallel.
     */
    struct k_sem lock;

    /**
     * Pointer to the incoming message buffer (request or desire, provided by process function)
     */
    const uint8_t *msg;

    /**
     * Length of the incoming message
     */
    size_t msg_len;

    /**
     * Position in the message currently being parsed
     */
    size_t msg_pos;

    /**
     * Pointer to the start of the payload in the message buffer
     */
    const uint8_t *msg_payload;

    /**
     * Pointer to the response buffer (provided by process function)
     */
    uint8_t *rsp;

    /**
     * Size of response buffer (i.e. maximum length)
     */
    size_t rsp_size;

    /**
     * Current position inside the response (equivalent to length of the response at end of
     * processing)
     */
    size_t rsp_pos;

    /**
     * Function pointers to mode-specific implementation (text or binary)
     */
    struct thingset_api *api;

    /**
     * State information for data processing, either for text mode or binary mode depending on the
     * assigned api.
     */
    union {
        /* Text mode */
        struct
        {
            /** JSON tokens in msg_payload parsed by JSMN */
            jsmntok_t tokens[CONFIG_THINGSET_NUM_JSON_TOKENS];

            /** Number of JSON tokens parsed by JSMN */
            size_t tok_count;

            /** Current position of the parsing process */
            size_t tok_pos;
        };
        /* Binary mode */
        struct
        {
            /** CBOR encoder states for binary mode */
            zcbor_state_t encoder[CONFIG_THINGSET_BINARY_MAX_DEPTH];

            /** CBOR decoder states for binary mode */
            zcbor_state_t decoder[CONFIG_THINGSET_BINARY_MAX_DEPTH];
        };
    };

    /**
     * Stores current authentication status (authentication as "normal" user as default)
     */
    uint8_t auth_flags;

    /**
     * Stores current authentication status (authentication as "normal" user as default)
     */
    uint8_t update_subsets;

    /**
     * Callback to be called from patch function if a value belonging to update_subsets
     * was changed
     */
    void (*update_cb)(void);

    /**
     * Endpoint used for the current message
     */
    struct thingset_endpoint endpoint;
};
