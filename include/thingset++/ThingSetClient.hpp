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
    template <typename Result, typename... TArg> bool exec(uint16_t id, Result *result, TArg... args)
    {
        return doRequest(id, ThingSetRequestType::exec, [&](auto encoder) { return encoder->encodeList(args...); }, result);
    }

    /// @brief Execute a function which does not return a value.
    /// @tparam ...TArg The types of the arguments to the function.
    /// @param id The integer identifier of the function.
    /// @param ...args The arguments to pass to the function.
    /// @return True if the function was successfully executed, otherwise false.
    template <typename... TArg> bool exec(uint16_t id, TArg... args)
    {
        return doRequest(id, ThingSetRequestType::exec, [&](auto encoder) { return encoder->encodeList(args...); }, nullptr);
    }

    /// @brief Gets a value.
    /// @tparam T The type of the value to get.
    /// @param id The integer identifier of the value.
    /// @param result A reference to a variable which will hold the retrieved value.
    /// @return True if retrieval succeeded, otherwise false.
    template <typename T> bool get(uint16_t id, T &result)
    {
        return doRequest(id, ThingSetRequestType::get, [](auto) { return true; }, &result);
    }

private:
    /// @brief Core RPC function.
    /// @tparam T The type of the value returned by an invocation, if any.
    /// @param id The integer identifier of the value to get or function to invoke, etc.
    /// @param type The request type.
    /// @param encode A function to encode any additional data in a request.
    /// @param result A pointer to a variable which will contain the decoded result, if any. Pass null if no result is expected.
    /// @return True if invocation succeeded, otherwise false.
    template <typename T> bool doRequest(uint16_t id, ThingSetRequestType type, std::function<bool (ThingSetBinaryEncoder*)> encode, T *result)
    {
        _txBuffer[0] = type;
        FixedDepthThingSetBinaryEncoder encoder(_txBuffer + 1, _txBufferSize - 1);
        if (!encoder.encode(id) || !encode(&encoder)) {
            return false;
        }

        if (!_transport.write(_txBuffer, 1 + encoder.getEncodedLength())) {
            LOG_ERR("Failed to send request");
            return false;
        }

        int responseSize;
        uint8_t *responseBuffer;
        if (!read(&responseBuffer, responseSize)) {
            LOG_ERR("Failed to read response");
            return false;
        }

        // if we have been passed a pointer for a result, decode into it
        // (e.g. executing a void function passes null to skip this)
        if (result) {
            FixedDepthThingSetBinaryDecoder decoder(responseBuffer, responseSize);
            return decoder.decode(result);
        }

        return true;
    }

    bool read(uint8_t **responseBuffer, int &responseSize);
};

} // namespace ThingSet
