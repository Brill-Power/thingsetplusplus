/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "ThingSetBinaryDecoder.hpp"
#include "ThingSetBinaryEncoder.hpp"
#include "ThingSetClientTransport.hpp"
#include "ThingSetStatus.hpp"

namespace ThingSet {

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

    template <typename... TArg> bool exec(uint16_t id, TArg... args)
    {
        _txBuffer[0] = ThingSetRequestType::exec;
        FixedDepthThingSetBinaryEncoder encoder(_txBuffer + 1, _txBufferSize);
        if (!encoder.encode(id) || !encoder.encodeList(args...)) {
            return false;
        }
        if (!_transport.write(_txBuffer, 1 + encoder.getEncodedLength())) {
            return false;
        }
    }

    template <typename T> bool get(uint16_t id, T &result)
    {
        _txBuffer[0] = ThingSetRequestType::get;
        FixedDepthThingSetBinaryEncoder encoder(_txBuffer + 1, _txBufferSize - 1);
        if (!encoder.encode(id)) {
            return false;
        }
        if (!_transport.write(_txBuffer, 1 + encoder.getEncodedLength())) {
            return false;
        }
        int read = _transport.read(_rxBuffer, _rxBufferSize);
        if (read > 1) {
            FixedDepthThingSetBinaryDecoder decoder(_rxBuffer + 1, read - 1);
            uint32_t id;
            decoder.decode(&id);
            return decoder.decode(result);
        }
        return false;
    }
};

} // namespace ThingSet
