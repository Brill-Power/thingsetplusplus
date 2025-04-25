/*
 * Copyright (c) 2024-2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "thingset++/ThingSetClient.hpp"

ThingSet::ThingSetClient::ThingSetClient(ThingSetClientTransport &transport, uint8_t *rxBuffer, size_t rxBufferSize,
                                         uint8_t *txBuffer, size_t txBufferSize)
    : _transport(transport), _rxBuffer(rxBuffer), _rxBufferSize(rxBufferSize), _txBuffer(txBuffer),
      _txBufferSize(txBufferSize)
{}

bool ThingSet::ThingSetClient::connect()
{
    return _transport.connect();
}

bool ThingSet::ThingSetClient::read(uint8_t **responseBuffer, int &responseSize)
{
    responseSize = _transport.read(_rxBuffer, _rxBufferSize);
    if (responseSize == 0) {
        return false;
    }

#ifdef DEBUG_LOGGING
    for (size_t i = 0; i < responseSize; i++)
    {
        printf("%.2x", _rxBuffer[i]);
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