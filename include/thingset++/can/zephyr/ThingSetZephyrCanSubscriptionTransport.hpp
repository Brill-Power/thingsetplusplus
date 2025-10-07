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

template <template<Frame> typename Base>
    requires std::is_base_of_v<_ThingSetCanSubscriptionTransport, Base> && std::is_base_of_v<ThingSetSubscriptionTransport<CanID>, Base>
class _ThingSetZephyrCanSubscriptionTransport : public Base<CanFrame>
{
protected:
    class _ZephyrCanSubscriptionListener : protected SubscriptionListener
    {
    private:
        const device *const _canDevice;
        int _filterId;
        ThingSet::Zephyr::MessageQueue<can_frame, CONFIG_THINGSET_PLUS_PLUS_CAN_SUBSCRIPTION_QUEUE_DEPTH> _frameQueue;
        k_thread _thread;
        std::function<void(const CanID &, ThingSetBinaryDecoder &)> _callback;
        K_KERNEL_STACK_MEMBER(_threadStack, CONFIG_THINGSET_PLUS_PLUS_CAN_SUBSCRIPTION_THREAD_STACK_SIZE);

    public:
        _ZephyrCanSubscriptionListener(const device *const canDevice) : _canDevice(canDevice), _filterId(-1)
        {}

        ~_ZephyrCanSubscriptionListener()
        {
            if (_filterId >= 0) {
                can_remove_rx_filter(_canDevice, _filterId);
            }
        }

        bool run(std::function<void(const CanID &, ThingSetBinaryDecoder &)> callback)
        {
            const CanID &canId = getCanIdForFilter();
            const can_filter canFilter = {
                .id = canId,
                .mask = canId.getMask(),
                .flags = CAN_FILTER_IDE,
            };
            _filterId = can_add_rx_filter(_canDevice, onPublicationFrameReceived, this, &canFilter);
            _callback = callback;
            k_thread_create(&_thread, this->threadStack, K_THREAD_STACK_SIZEOF(this->threadStack), runListener, this, nullptr, nullptr, CONFIG_THINGSET_PLUS_PLUS_CAN_SUBSCRIPTION_THREAD_PRIORITY, 0, K_NO_WAIT);
            return true;
        }

    protected:
        virtual void runListener() = 0;
        virtual const CanID &getCanIdForFilter() const = 0;

    private:
        static void runListener(void *param1, void *, void *)
        {
            auto self = (_ZephyrCanSubscriptionListener *)param1;
            self->runListener();
        }

        static void onPublicationFrameReceived(const device *canDevice, can_frame *frame, void *userData)
        {
            auto self = (_ZephyrCanSubscriptionListener *)userData;
            self->onPublicationFrameReceived(frame);
        }

        void onPublicationFrameReceived(can_frame *frame)
        {
            if (!_frameQueue.push(*frame))
            {
                LOG_ERROR("Failed to push frame to queue");
            }
        }
    };

    ThingSetZephyrCanInterface &_canInterface;

public:
    _ThingSetZephyrCanSubscriptionTransport(ThingSetZephyrCanInterface &canInterface) : _canInterface(canInterface)
    {}

    bool subscribe(std::function<void(const CanID &, ThingSetBinaryDecoder &)> callback) override
    {
        return _listener.run(callback);
    }

protected:
    ThingSetCanInterface &getInterface() override
    {
        return _canInterface;
    }
};

class ThingSetZephyrCanSubscriptionTransport : public _ThingSetZephyrCanSubscriptionTransport<ThingSetCanSubscriptionTransport>
{
private:
    class ZephyrCanSubscriptionListener : public _ZephyrCanSubscriptionListener
    {
    public:
        ZephyrCanSubscriptionListener(const device *const canDevice);

    protected:
        void runListener() override;
        const CanID &getCanIdForFilter() const override;
    };

    ZephyrCanSubscriptionListener _listener;

public:
    ThingSetZephyrCanSubscriptionTransport(ThingSetZephyrCanInterface &canInterface);
};

class ThingSetZephyrCanControlSubscriptionTransport : public _ThingSetZephyrCanSubscriptionTransport<ThingSetCanControlSubscriptionTransport>
{
};

} // namespace ThingSet::Can::Zephyr