/*
 * Copyright (c) 2024-2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "thingset++/ThingSetClient.hpp"
#include "thingset++/ThingSetRegistry.hpp"

namespace ThingSet {

static constexpr uint8_t cborNull = 0xF6;
static constexpr size_t responseHeaderSize = 2; /* status code + CBOR null */

ThingSetClient::ThingSetClient(ThingSetClientTransport &transport, uint8_t *rxBuffer, size_t rxBufferSize,
                                         uint8_t *txBuffer, size_t txBufferSize)
    : _transport(transport), _rxBuffer(rxBuffer), _rxBufferSize(rxBufferSize), _txBuffer(txBuffer),
      _txBufferSize(txBufferSize)
{}

bool ThingSetClient::connect()
{
    return _transport.connect();
}

ThingSetResult ThingSetClient::read(uint8_t **responseBuffer, size_t &responseSize)
{
    responseSize = 0;

    int received = _transport.read(_rxBuffer, _rxBufferSize);
    if (received <= 0) {
        // No response (0) or a transport error such as a receive timeout
        // (negative errno). The rx buffer may still hold a previous response
        return ThingSetResult(ThingSetStatusCode::gatewayTimeout);
    }

#ifdef DEBUG_LOGGING
    for (int i = 0; i < received; i++)
    {
        if (i > 0 && i % 16 == 0) {
            printf("\n");
        }
        printf("%.2x ", _rxBuffer[i]);
    }
    printf("\n");
#endif

    ThingSetResult result = ThingSetResult((ThingSetStatusCode)_rxBuffer[0]);
    if (!result) {
        return result;
    }

    // a successful response carries at least the status code plus a CBOR null
    if ((size_t)received < responseHeaderSize || _rxBuffer[1] != cborNull) {
        return ThingSetResult(ThingSetStatusCode::internalServerError);
    }

    // return size having accounted for response code and null
    responseSize = (size_t)received - responseHeaderSize;
    *responseBuffer = &_rxBuffer[responseHeaderSize];

    return result;
}

} // namespace ThingSet