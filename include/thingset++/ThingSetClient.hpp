/*
 * Copyright (c) 2024-2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "thingset++/ThingSetBinaryDecoder.hpp"
#include "thingset++/ThingSetBinaryEncoder.hpp"
#include "thingset++/ThingSetClientTransport.hpp"
#include "thingset++/ThingSetStatus.hpp"
#include "thingset++/internal/logging.hpp"

namespace ThingSet {

/// @brief ThingSet client.
class ThingSetClient
{
private:
    ThingSetClientTransport &_transport;
    uint8_t *_rxBuffer;
    const size_t _rxBufferSize;
    uint8_t *_txBuffer;
    const size_t _txBufferSize;

public:
    ThingSetClient(ThingSetClient &&) = delete;
    ThingSetClient(const ThingSetClient &) = delete;
    ThingSetClient(ThingSetClientTransport &transport, uint8_t *rxBuffer, const size_t rxBufferSize, uint8_t *txBuffer,
                   const size_t txBufferSize);
    template <size_t RxSize, size_t TxSize>
    ThingSetClient(ThingSetClientTransport &transport, std::array<uint8_t, RxSize> &rxBuffer,
                   std::array<uint8_t, TxSize> &txBuffer)
        : ThingSetClient(transport, rxBuffer.data(), rxBuffer.size(), txBuffer.data(), txBuffer.size())
    {}

    bool connect();

    /// @brief Execute a function which returns a value.
    /// @tparam Result The type of the return value of the function.
    /// @tparam ...TArg The types of the arguments to the function.
    /// @param id The integer identifier of the function.
    /// @param result A pointer to a variable which will hold the decoded return value.
    /// @param ...args The arguments to pass to the function.
    /// @return True if the function was successfully executed, otherwise false.
    template <typename Result, typename... TArg> bool exec(const uint16_t &id, Result *result, TArg... args)
    {
        return doRequest(id, ThingSetBinaryRequestType::exec, [&](auto encoder) { return encoder->encodeList(args...); }, result);
    }

    /// @brief Execute a function which does not return a value.
    /// @tparam ...TArg The types of the arguments to the function.
    /// @param id The integer identifier of the function.
    /// @param ...args The arguments to pass to the function.
    /// @return True if the function was successfully executed, otherwise false.
    template <typename... TArg> bool exec(const uint16_t &id, TArg... args)
    {
        return doRequest(id, ThingSetBinaryRequestType::exec, [&](auto encoder) { return encoder->encodeList(args...); });
    }

    /// @brief Gets a value.
    /// @tparam T The type of the value to get.
    /// @param id The integer identifier of the value.
    /// @param result A reference to a variable which will hold the retrieved value.
    /// @return True if retrieval succeeded, otherwise false.
    template <typename T> bool get(const uint16_t &id, T &result)
    {
        return doRequest(id, ThingSetBinaryRequestType::get, [](auto) { return true; }, &result);
    }

    /// @brief Gets a value.
    /// @tparam T The type of the value to get.
    /// @param id The string identifier of the value.
    /// @param result A reference to a variable which will hold the retrieved value.
    /// @return True if retrieval succeeded, otherwise false.
    template <typename T> bool get(const std::string &id, T &result)
    {
        return doRequest(id, ThingSetBinaryRequestType::get, [](auto) { return true; }, &result);
    }

    template <typename Id>
        requires std::is_integral_v<Id> or std::is_convertible_v<Id, std::string_view>
    bool fetch(const Id &id, std::vector<Id> &result)
    {
        return doRequest(id, ThingSetBinaryRequestType::fetch, [](auto encoder) { return encoder->encodeNull(); }, &result);
    }

    template <typename T>
    bool update(const uint16_t &id, const T &value)
    {
        // use root node as parent ID for ID-based updates
        return doRequest(0, ThingSetBinaryRequestType::update, [&](auto encoder) {
            return encoder->encodeMapStart() &&
                encoder->encode(id) &&
                encoder->encode(value) &&
                encoder->encodeMapEnd();
        });
    }

    template <typename T> bool update(const std::string &fullyQualifiedName, const T &value)
    {
        size_t index = fullyQualifiedName.find_last_of('/');
        const std::string pathToParent = index != std::string::npos ? fullyQualifiedName.substr(0, index) : "";
        const std::string key = fullyQualifiedName.substr(index + 1);
        return doRequest(pathToParent, ThingSetBinaryRequestType::update, [&](auto encoder) {
            return encoder->encodeMapStart() &&
                encoder->encode(key) &&
                encoder->encode(value) &&
                encoder->encodeMapEnd();
        });
    }

private:
    /// @brief Core RPC function.
    /// @tparam T The type of the value returned by an invocation, if any.
    /// @param id The integer identifier of the value to get or function to invoke, etc.
    /// @param type The request type.
    /// @param encode A function to encode any additional data in a request.
    /// @param result A pointer to a variable which will contain the decoded result, if any. Pass null if no result is expected.
    /// @return True if invocation succeeded, otherwise false.
    template <typename Id, typename T>
        requires std::is_integral_v<Id> or std::is_convertible_v<Id, std::string_view>
    bool doRequest(const Id &id, ThingSetBinaryRequestType type, std::function<bool (ThingSetBinaryEncoder *)> encode, T *result)
    {
        uint8_t *responseBuffer;
        size_t responseSize;
        if (doRequestCore(id, type, encode, &responseBuffer, responseSize)) {
            // decode result into pointer
            FixedDepthThingSetBinaryDecoder decoder(responseBuffer, responseSize);
            return decoder.decode(result);
        }
        return false;
    }

    template <typename Id>
        requires std::is_integral_v<Id> or std::is_convertible_v<Id, std::string_view>
    bool doRequest(const Id &id, ThingSetBinaryRequestType type, std::function<bool (ThingSetBinaryEncoder *)> encode)
    {
        uint8_t *responseBuffer;
        size_t responseSize;
        return doRequestCore(id, type, encode, &responseBuffer, responseSize);
    }

    template <typename Id>
        requires std::is_integral_v<Id> or std::is_convertible_v<Id, std::string_view>
    bool doRequestCore(const Id &id, ThingSetBinaryRequestType type, std::function<bool (ThingSetBinaryEncoder *)> encode, uint8_t **responseBuffer, size_t &responseSize)
    {
        _txBuffer[0] = (uint8_t)type;
        FixedDepthThingSetBinaryEncoder encoder(_txBuffer + 1, _txBufferSize - 1);
        if (!encoder.encode(id) || !encode(&encoder)) {
            return false;
        }

        if (!_transport.write(_txBuffer, 1 + encoder.getEncodedLength())) {
            LOG_ERROR("Failed to send request");
            return false;
        }

        if (!read(responseBuffer, responseSize)) {
            LOG_ERROR("Failed to read response");
            return false;
        }

        return true;
    }

    bool read(uint8_t **responseBuffer, size_t &responseSize);
};

} // namespace ThingSet
