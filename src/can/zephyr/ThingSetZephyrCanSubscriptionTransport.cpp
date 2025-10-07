/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "thingset++/can/zephyr/ThingSetZephyrCanSubscriptionTransport.hpp"

namespace ThingSet::Can::Zephyr {

// not sure what to do with this at the moment
static K_THREAD_STACK_DEFINE(threadStack, CONFIG_THINGSET_PLUS_PLUS_CAN_SUBSCRIPTION_THREAD_STACK_SIZE);

ThingSetZephyrCanSubscriptionTransport::ThingSetZephyrCanSubscriptionTransport(ThingSetZephyrCanInterface &canInterface) : _canInterface(canInterface), _listener(canInterface.getDevice())
{}

ThingSetZephyrCanSubscriptionTransport::ZephyrCanSubscriptionListener::ZephyrCanSubscriptionListener(const device *const canDevice) : _canDevice(canDevice), _filterId(-1)
{
}

ThingSetZephyrCanSubscriptionTransport::ZephyrCanSubscriptionListener::~ZephyrCanSubscriptionListener()
{
    if (_filterId >= 0) {
        can_remove_rx_filter(_canDevice, _filterId);
    }
}

void ThingSetZephyrCanSubscriptionTransport::ZephyrCanSubscriptionListener::onPublicationFrameReceived(can_frame *frame)
{
    if (!_frameQueue.push(*frame))
    {
        // now what?
    }
}

void ThingSetZephyrCanSubscriptionTransport::ZephyrCanSubscriptionListener::onControlFrameReceived(can_frame *frame)
{
    if (!_controlQueue.push(*frame))
    {
        // now what?
    }
}

void ThingSetZephyrCanSubscriptionTransport::ZephyrCanSubscriptionListener::onPublicationFrameReceived(const device *canDevice, can_frame *frame, void *userData)
{
    auto self = (ZephyrCanSubscriptionListener *)userData;
    self->onPublicationFrameReceived(frame);
}

void ThingSetZephyrCanSubscriptionTransport::ZephyrCanSubscriptionListener::onControlFrameReceived(const device *canDevice, can_frame *frame, void *userData)
{
    auto self = (ZephyrCanSubscriptionListener *)userData;
    self->onControlFrameReceived(frame);
}

void ThingSetZephyrCanSubscriptionTransport::ZephyrCanSubscriptionListener::runListener(ThingSet::Zephyr::MessageQueue<can_frame, CONFIG_THINGSET_PLUS_PLUS_CAN_SUBSCRIPTION_QUEUE_DEPTH> &_queue)
{
    std::map<uint8_t, StreamingCanThingSetBinaryDecoder<CanFrame>> decodersByNodeAddress;
    while (true)
    {
        can_frame rawFrame = _queue.pop();
        CanFrame frame(rawFrame);
        handle(frame, frame.getId(), frame.getId().getSource(), decodersByNodeAddress, _callback);
    }
}

void ThingSetZephyrCanSubscriptionTransport::ZephyrCanSubscriptionListener::runListener(void *param1, void *param2, void *)
{
    auto self = (ZephyrCanSubscriptionListener *)param1;
    self->runListener(((bool *)param2) ? self->_controlQueue : self->_frameQueue);
}

bool ThingSetZephyrCanSubscriptionTransport::ZephyrCanSubscriptionListener::run(std::function<void(const CanID &, ThingSetBinaryDecoder &)> callback, bool singleFrame)
{
    const can_filter canFilter = {
        .id = (singleFrame) ? singleFrameReportFilter : subscriptionFilter,
        .mask = ((singleFrame) ? singleFrameReportFilter : subscriptionFilter).getMask(),
        .flags = CAN_FILTER_IDE,
    };

    if (singleFrame)
    {
        _filterId = can_add_rx_filter(_canDevice, onControlFrameReceived, this, &canFilter);
    }
    else {
        _filterId = can_add_rx_filter(_canDevice, onPublicationFrameReceived, this, &canFilter);
    }

    _callback = callback;
    k_thread_create(&_thread, threadStack, K_THREAD_STACK_SIZEOF(threadStack), runListener, this, (void *)singleFrame, nullptr, CONFIG_THINGSET_PLUS_PLUS_CAN_SUBSCRIPTION_THREAD_PRIORITY, 0, K_NO_WAIT);
    return true;
}

bool ThingSetZephyrCanSubscriptionTransport::subscribe(std::function<void(const CanID &, ThingSetBinaryDecoder &)> callback)
{
    return _listener.run(callback, false);
}

bool ThingSetZephyrCanSubscriptionTransport::subscribeSingleFrame(std::function<void(const CanID &, ThingSetBinaryDecoder &)> callback)
{
    return _listener.run(callback, true);
}

ThingSetCanInterface &ThingSetZephyrCanSubscriptionTransport::getInterface()
{
    return _canInterface;
}

} // namespace ThingSet::Can::Zephyr