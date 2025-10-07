/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "thingset++/can/zephyr/ThingSetZephyrCanSubscriptionTransport.hpp"
#include "thingset++/internal/logging.hpp"

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

const CanID &ThingSetZephyrCanSubscriptionTransport::ZephyrCanSubscriptionListener::getCanIdForFilter() const
{
    return subscriptionFilter;
}

} // namespace ThingSet::Can::Zephyr