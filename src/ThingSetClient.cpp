/*
 * Copyright (c) 2024 Brill Power.
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
