/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <cstdint>
#include <thingset++/can/CanConfig.hpp>
#ifndef __ZEPHYR__
#include <iostream>
#endif // __ZEPHYR__

#define THINGSET_CAN_ID_POSITION_SOURCE   0
#define THINGSET_CAN_ID_POSITION_TARGET   8
#define THINGSET_CAN_ID_POSITION_DATA_ID  8
#define THINGSET_CAN_ID_POSITION_SEQ_NO   8
#define THINGSET_CAN_ID_POSITION_MF_TYPE  12
#define THINGSET_CAN_ID_POSITION_MSG_NO   14
#define THINGSET_CAN_ID_POSITION_BRIDGE   16
#define THINGSET_CAN_ID_POSITION_TYPE     24
#define THINGSET_CAN_ID_POSITION_PRIORITY 26

namespace ThingSet::Can {

/// @brief Specifies the type of a message.
enum MessageType
{
    /// @brief Request-response. Uses ISO-TP.
    requestResponse = 0x0 << THINGSET_CAN_ID_POSITION_TYPE,
    /// @brief Report spread over multiple frames.
    multiFrameReport = 0x1 << THINGSET_CAN_ID_POSITION_TYPE,
    /// @brief Single frame report, also known as a control message.
    singleFrameReport = 0x2 << THINGSET_CAN_ID_POSITION_TYPE,
    /// @brief Network management messages (address claims, etc.).
    network = 0x3 << THINGSET_CAN_ID_POSITION_TYPE,
};

/// @brief Specifies the sub-type of a multi-frame report.
enum MultiFrameMessageType
{
    /// @brief The first frame in a multi-frame report.
    first = 0x0 << THINGSET_CAN_ID_POSITION_MF_TYPE,
    /// @brief A frame that is neither the first nor the last in a multi-frame report.
    consecutive = 0x1 << THINGSET_CAN_ID_POSITION_MF_TYPE,
    /// @brief The last frame in a multi-frame report.
    last = 0x2 << THINGSET_CAN_ID_POSITION_MF_TYPE,
    /// @brief A multi-frame report-type message that fits inside a single frame.
    single = 0x3 << THINGSET_CAN_ID_POSITION_MF_TYPE,
};

/// @brief Specifies the priority of a message.
enum MessagePriority
{
    controlEmergency = 0x0 << THINGSET_CAN_ID_POSITION_PRIORITY,
    controlHigh = 0x2 << THINGSET_CAN_ID_POSITION_PRIORITY,
    controlLow = 0x3 << THINGSET_CAN_ID_POSITION_PRIORITY,
    networkManagement = 0x4 << THINGSET_CAN_ID_POSITION_PRIORITY,
    reportHigh = 0x5 << THINGSET_CAN_ID_POSITION_PRIORITY,
    channel = 0x6 << THINGSET_CAN_ID_POSITION_PRIORITY,
    reportLow = 0x7 << THINGSET_CAN_ID_POSITION_PRIORITY
};

/// @brief Encapsulates a CAN ID as used by ThingSet.
class CanID
{
private:
    uint32_t _id;
    uint32_t _mask;

public:
    /// @brief The minimum valid address for a node.
    static const uint8_t minAddress = CanConfig::minAddress;
    /// @brief The maximum valid address for a node.
    static const uint8_t maxAddress = CanConfig::maxAddress;
    /// @brief The anonymous node address.
    static const uint8_t anonymousAddress = 0xfe;
    /// @brief The broadcast node address.
    static const uint8_t broadcastAddress = 0xff;
    /// @brief The default (local) bridge address.
    static const uint8_t defaultBridge = 0x00;

private:
    static const uint32_t sourceMask = 0xff << THINGSET_CAN_ID_POSITION_SOURCE;
    static const uint32_t targetMask = 0xff << THINGSET_CAN_ID_POSITION_TARGET;
    static const uint32_t dataIdMask = 0xffff << THINGSET_CAN_ID_POSITION_DATA_ID;
    static const uint32_t sequenceNumberMask = 0xf << THINGSET_CAN_ID_POSITION_SEQ_NO;
    static const uint32_t multiFrameTypeMask = 0x3 << THINGSET_CAN_ID_POSITION_MF_TYPE;
    static const uint32_t messageNumberMask = 0x3 << THINGSET_CAN_ID_POSITION_MSG_NO;
    static const uint32_t bridgeMask = 0xff << THINGSET_CAN_ID_POSITION_BRIDGE;
    static const uint32_t messageTypeMask = 0x3 << THINGSET_CAN_ID_POSITION_TYPE;
    static const uint32_t messagePriorityMask = 0x7 << THINGSET_CAN_ID_POSITION_PRIORITY;

public:
    CanID();
    CanID(const uint32_t id, const uint32_t mask);
    CanID(const CanID &other);

    operator uint32_t() const;

    /// @brief Gets the CAN ID as a value.
    uint32_t getId() const;
    uint32_t getIdWithFlags() const;
    /// @brief Gets a mask that indicates the set bits of this CAN ID.
    uint32_t getMask() const;
    CanID &setMask(const uint32_t mask);

    /// @brief Gets the reply address equivalent of this CAN ID.
    CanID getReplyId() const;

    /// @brief Gets the source component of this CAN ID.
    uint8_t getSource() const;
    /// @brief Sets the source component of this CAN ID.
    /// @return The modified CAN ID.
    CanID &setSource(const uint8_t value);
    /// @brief Gets the target component of this CAN ID.
    uint8_t getTarget() const;
    /// @brief Sets the target component of this CAN ID.
    /// @return The modified CAN ID.
    CanID &setTarget(const uint8_t value);

    uint16_t getDataID() const;
    CanID &setDataID(const uint16_t value);

    uint8_t getSequenceNumber() const;
    CanID &setSequenceNumber(const uint8_t value);
    uint8_t getMessageNumber() const;
    CanID &setMessageNumber(const uint8_t value);
    uint8_t getBridge() const;
    CanID &setBridge(const uint8_t value);
    uint8_t getRandomElement() const;
    CanID &setRandomElement(const uint8_t value);

    MessageType getMessageType() const;
    CanID &setMessageType(const MessageType value);

    MultiFrameMessageType getMultiFrameMessageType() const;
    CanID &setMultiFrameMessageType(const MultiFrameMessageType value);

    MessagePriority getMessagePriority() const;
    CanID &setMessagePriority(const MessagePriority value);

    static CanID create(const uint32_t id);
};

#ifndef __ZEPHYR__
std::ostream& operator<<(std::ostream &os, const CanID &id);
#endif // __ZEPHYR__

} // namespace ThingSet::Can