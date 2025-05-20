/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "thingset++/ThingSetSubscriptionTransport.hpp"
#include "thingset++/can/ThingSetCanInterface.hpp"
#include "thingset++/can/StreamingCanThingSetBinaryDecoder.hpp"
#include "thingset++/ThingSetStatus.hpp"

namespace ThingSet::Can {

class _ThingSetCanSubscriptionTransport
{
protected:
    static const CanID subscriptionFilter;
};

template <typename Frame>
class ThingSetCanSubscriptionTransport : public _ThingSetCanSubscriptionTransport, public ThingSetSubscriptionTransport<CanID>
{
protected:
    class SubscriptionListener {
        protected:
            bool handle(Frame &frame, std::map<uint8_t, StreamingCanThingSetBinaryDecoder<Frame>> &decodersByNodeAddress, std::function<void(const CanID &, ThingSetBinaryDecoder &)>& callback)
            {
                uint8_t sender = frame.getId().getSource();
                StreamingCanThingSetBinaryDecoder<Frame> *decoder = nullptr;
                if (frame.getData()[0] == ThingSetRequestType::report) {
                    // new report, so create a new decoder and add to map
                    decodersByNodeAddress[sender] = StreamingCanThingSetBinaryDecoder<Frame>();
                    decoder = &decodersByNodeAddress[sender];
                } else {
                    auto found = decodersByNodeAddress.find(sender);
                    if (found != decodersByNodeAddress.end()) {
                        decoder = &found->second;
                    }
                }
                if (decoder && decoder->enqueue(std::move(frame))) {
                    callback(frame.getId(), *decoder);
                }
                return true;
            }
    };

protected:
    virtual ThingSetCanInterface &getInterface() = 0;
};

} // namespace ThingSet::Can