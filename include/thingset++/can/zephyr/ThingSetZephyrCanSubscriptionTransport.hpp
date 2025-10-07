/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "thingset++/can/ThingSetCanSubscriptionTransport.hpp"
#include "thingset++/can/zephyr/ThingSetZephyrCanInterface.hpp"
#include "thingset++/can/zephyr/CanFrame.hpp"
#include "thingset++/zephyr/MessageQueue.hpp"
#include <functional>

namespace ThingSet::Can::Zephyr {

class ThingSetZephyrCanSubscriptionTransport : public ThingSetCanSubscriptionTransport<CanFrame>
{
private:
    class ZephyrCanSubscriptionListener : protected SubscriptionListener
    {
    private:
        const device *const _canDevice;
        int _filterId;
        ThingSet::Zephyr::MessageQueue<can_frame, CONFIG_THINGSET_PLUS_PLUS_CAN_SUBSCRIPTION_QUEUE_DEPTH> _frameQueue;
        k_thread _thread;
        K_THREAD_STACK_MEMBER(threadStack, CONFIG_THINGSET_PLUS_PLUS_CAN_SUBSCRIPTION_THREAD_STACK_SIZE);
        std::function<void(const CanID &, ThingSetBinaryDecoder &)> _callback;

    public:
        ZephyrCanSubscriptionListener(const device *const canDevice);
        ~ZephyrCanSubscriptionListener();

        bool run(std::function<void(const CanID &, ThingSetBinaryDecoder &)> callback, bool singleFrame);

    private:
        static void runListener(void *param1, void *, void *);
        void runListener();

        static void onPublicationFrameReceived(const device *canDevice, can_frame *frame, void *userData);
        void onPublicationFrameReceived(can_frame *frame);

    };

    ThingSetZephyrCanInterface &_canInterface;
    ZephyrCanSubscriptionListener _listener;

public:
    ThingSetZephyrCanSubscriptionTransport(ThingSetZephyrCanInterface &canInterface);

    bool subscribe(std::function<void(const CanID &, ThingSetBinaryDecoder &)> callback) override;
    bool subscribeSingleFrame(std::function<void(const CanID &, ThingSetBinaryDecoder &)> callback);

protected:
    ThingSetCanInterface &getInterface() override;
};

} // namespace ThingSet::Can::Zephyr