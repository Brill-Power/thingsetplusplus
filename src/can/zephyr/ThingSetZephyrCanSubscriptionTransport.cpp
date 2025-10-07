/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "thingset++/can/zephyr/ThingSetZephyrCanSubscriptionTransport.hpp"
#include "thingset++/ThingSetBinaryDecoder.hpp"
#include <zephyr/sys/byteorder.h>

namespace ThingSet::Can::Zephyr {

ThingSetZephyrCanSubscriptionTransport::ThingSetZephyrCanSubscriptionTransport(ThingSetZephyrCanInterface &canInterface) : _ThingSetZephyrCanSubscriptionTransport<ThingSetCanSubscriptionTransport>(canInterface), _listener(canInterface.getDevice())
{}

ThingSetZephyrCanSubscriptionTransport::ZephyrCanSubscriptionListener::ZephyrCanSubscriptionListener(const device *const canDevice) : _ZephyrCanSubscriptionListener(canDevice)
{
}

void ThingSetZephyrCanSubscriptionTransport::ZephyrCanSubscriptionListener::runListener()
{
    std::map<uint8_t, StreamingCanThingSetBinaryDecoder<CanFrame>> decodersByNodeAddress;
    while (true)
    {
        can_frame rawFrame = _frameQueue.pop();
        CanFrame frame(rawFrame);
        handle(frame, frame.getId(), frame.getId().getSource(), decodersByNodeAddress, _callback);
    }
}

bool ThingSetZephyrCanSubscriptionTransport::subscribe(std::function<void(const CanID &, ThingSetBinaryDecoder &)> callback)
{
    return _listener.run(callback);
}

const CanID &ThingSetZephyrCanSubscriptionTransport::ZephyrCanSubscriptionListener::getCanIdForFilter() const
{
    return reportFilter;
}

ThingSetZephyrCanControlSubscriptionTransport::ThingSetZephyrCanSubscriptionTransport(ThingSetZephyrCanInterface &canInterface) : _ThingSetZephyrCanSubscriptionTransport<ThingSetCanControlSubscriptionTransport>(canInterface), _listener(canInterface.getDevice())
{}

ThingSetZephyrCanControlSubscriptionTransport::ZephyrCanSubscriptionListener::ZephyrCanSubscriptionListener(const device *const canDevice) : _ZephyrCanSubscriptionListener(canDevice)
{
}

void ThingSetZephyrCanControlSubscriptionTransport::ZephyrCanSubscriptionListener::runListener()
{
    const size_t MAP_HEADER_SIZE = 1;
    const size_t ITEM_ID_MAX_SIZE = 3;
    const size_t SUBSET_SIZE = 1;
    std::array<uint8_t, CANFD_MAX_DLEN + SUBSET_SIZE + MAP_HEADER_SIZE + ITEM_ID_MAX_SIZE> buffer;
    buffer[0] = 0x0; // dummy subset
    while (true)
    {
        can_frame rawFrame = _frameQueue.pop();
        CanFrame frame(rawFrame);
        uint16_t itemId = frame.getId().getDataId();
        size_t i = 1;
        buffer[i++] = 0xA1; // map with one entry
        if (itemId <= ZCBOR_VALUE_IN_HEADER) {
            buffer[i++] = itemId;
        }
        else if (itemId <= 0xFF)
        {
            buffer[i++] = 0x18;
            buffer[i++] = (uint8_t)itemId;
        }
        else
        {
            buffer[i++] = 0x19;
            sys_put_be16(itemId, &buffer[i]);
            i += 2;
        }
        memcpy(&buffer[i], frame.getData(), frame.getLength());
        FixedDepthThingSetBinaryDecoder<4> decoder(buffer);
        _callback(frame.getId(), decoder);
    }
}

bool ThingSetZephyrCanControlSubscriptionTransport::subscribe(std::function<void(const CanID &, ThingSetBinaryDecoder &)> callback)
{
    return _listener.run(callback);
}

const CanID &ThingSetZephyrCanControlSubscriptionTransport::ZephyrCanSubscriptionListener::getCanIdForFilter() const
{
    return controlFilter;
}

} // namespace ThingSet::Can::Zephyr