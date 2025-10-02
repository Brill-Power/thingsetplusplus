/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "thingset++/ThingSetServerTransport.hpp"
#include "thingset++/ip/StreamingUdpThingSetBinaryEncoder.hpp"

namespace ThingSet::Ip {

/// @brief Server transport for UDP protocol.
/// @tparam Identifier Type of client identifier
template <typename Identifier>
class ThingSetIpServerTransport : public ThingSetServerTransport<Identifier, THINGSET_STREAMING_MSG_SIZE, StreamingUdpThingSetBinaryEncoder<Identifier>>
{
protected:
    uint8_t _messageNumber;

    ThingSetIpServerTransport() : _messageNumber(0)
    {}

public:
    StreamingUdpThingSetBinaryEncoder<Identifier> getPublishingEncoder(bool enhanced) override {
        return StreamingUdpThingSetBinaryEncoder<Identifier>(*this, enhanced);
    }

    /// @brief Publish a request using a broadcast mechanism appropriate to the underlying
    /// transport.
    /// @param buffer A pointer to the buffer to be broadcasted.
    /// @param len The length of the data in the buffer.
    /// @return True.
    virtual bool publish(uint8_t *buffer, size_t len) = 0;
};

} // namespace ThingSet::Ip