/*
 * Copyright (c) 2024-2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "thingset++/ThingSetClient.hpp"
#include "thingset++/ThingSetRegistry.hpp"

namespace ThingSet {

ThingSetClient::ThingSetClient(ThingSetClientTransport &transport, uint8_t *rxBuffer, size_t rxBufferSize,
                                         uint8_t *txBuffer, size_t txBufferSize)
    : _transport(transport), _rxBuffer(rxBuffer), _rxBufferSize(rxBufferSize), _txBuffer(txBuffer),
      _txBufferSize(txBufferSize)
{}

bool ThingSetClient::connect()
{
    return _transport.connect();
}

bool ThingSetClient::read(uint8_t **responseBuffer, size_t &responseSize)
{
    responseSize = _transport.read(_rxBuffer, _rxBufferSize);
    if (responseSize == 0) {
        return false;
    }

#ifdef DEBUG_LOGGING
    for (size_t i = 0; i < responseSize; i++)
    {
        if (i > 0 && i % 16 == 0) {
            printf("\n");
        }
        printf("%.2x ", _rxBuffer[i]);
    }
    printf("\n");
#endif

    switch (_rxBuffer[0]) {
        case ThingSetStatusCode::content:
        case ThingSetStatusCode::changed:
        case ThingSetStatusCode::deleted:
            break;
        default:
            return false;
    }

    // first value is always a CBOR null
    if (_rxBuffer[1] != 0xF6) {
        return false;
    }

    // return size having accounted for response code and null
    responseSize -= 2;
    *responseBuffer = &_rxBuffer[2];

    return true;
}

} // namespace ThingSet