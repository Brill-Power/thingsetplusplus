/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <cstdint>
#include <functional>
#include "thingset++/Streaming.hpp"
#include "thingset++/ThingSetBinaryDecoder.hpp"
#include "thingset++/ThingSetStatus.hpp"

namespace ThingSet {

/// @brief Interface for all client subscription transports.
/// @tparam Identifier Type of client identifier
template <typename Identifier>
class ThingSetSubscriptionTransport
{
public:
    /// @brief Subscribes for publications delivered via the transport's
    /// broadcast mechanism.
    /// @param callback A callback that is invoked when a published message
    /// is received.
    virtual bool subscribe(std::function<void(const Identifier &, ThingSetBinaryDecoder &)> callback) = 0;
};

/// @brief Interface for client subscription transports receiving multi-frame reports.
/// @tparam Identifier Type of client identifier
template <typename Identifier, typename T>
concept SubscriptionTransport = std::is_base_of_v<ThingSetSubscriptionTransport<Identifier>, T>;

template <typename Identifier>
class ThingSetMultiFrameSubscriptionTransport : public ThingSetSubscriptionTransport<Identifier>
{
protected:
    class SubscriptionListener
    {
    public:
        template <typename Decoder, typename Message, typename Key = Identifier, StreamingMessageType MessageType = Decoder::message_type>
        static bool handle(Message &frame, const Identifier &identifier, const Key &key, std::map<Key, Decoder> &decodersBySender, std::function<void(const Identifier &, ThingSetBinaryDecoder &)>& callback)
        {
            Decoder *decoder = nullptr;
            MessageType messageType = getMessageType(frame);
            if (messageType == MessageType::first || messageType == MessageType::single) {
                // new report, so create a new decoder and add to map
                decodersBySender[key] = Decoder();
                decoder = &decodersBySender[key];
            } else {
                auto found = decodersBySender.find(key);
                if (found != decodersBySender.end()) {
                    decoder = &found->second;
                }
            }
            if (decoder && decoder->enqueue(std::move(frame))) {
                callback(identifier, *decoder);
            }
            return true;
        }
    };
};

} // namespace ThingSet