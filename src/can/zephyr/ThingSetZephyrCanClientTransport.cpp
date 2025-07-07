/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "thingset++/can/zephyr/ThingSetZephyrCanClientTransport.hpp"

namespace ThingSet::Can::Zephyr {

bool ThingSetZephyrCanClientTransport::connect()
{
    return _requestResponseContext.bind(_targetNodeAddress, [&](const CanID &sender, uint8_t *rxBuffer, size_t rxSize, uint8_t *, size_t)
    {
        ResponseMessage message;
        memcpy(message.buffer, rxBuffer, rxSize);
        message.length = rxSize;
        return k_msgq_put(&_responseQueue, &message, K_MSEC(100));
    });
}

int ThingSetZephyrCanClientTransport::read(uint8_t *buffer, size_t len)
{
    ResponseMessage message;
    int result = k_msgq_get(&_responseQueue, &message, K_SECONDS(1));
    if (result == 0)
    {
        memcpy(buffer, message.buffer, message.length);
        return message.length;
    }
    return result;
}

bool ThingSetZephyrCanClientTransport::write(uint8_t *buffer, size_t len)
{
    return _requestResponseContext.send(_targetNodeAddress, buffer, len);
}

} // namespace ThingSet::Can::Zephyr
