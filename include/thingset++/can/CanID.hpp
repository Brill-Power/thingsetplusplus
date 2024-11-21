/*
 * Copyright (c) 2024 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */
#pragma once

#include <cstdint>

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

enum MessageType
{
    requestResponse = 0x0 << THINGSET_CAN_ID_POSITION_TYPE,
    multiFrameReport = 0x1 << THINGSET_CAN_ID_POSITION_TYPE,
    singleFrameReport = 0x2 << THINGSET_CAN_ID_POSITION_TYPE,
    network = 0x3 << THINGSET_CAN_ID_POSITION_TYPE,
};

enum MultiFrameMessageType
{
    first = 0x0 << THINGSET_CAN_ID_POSITION_MF_TYPE,
    consecutive = 0x1 << THINGSET_CAN_ID_POSITION_MF_TYPE,
    last = 0x2 << THINGSET_CAN_ID_POSITION_MF_TYPE,
    single = 0x3 << THINGSET_CAN_ID_POSITION_MF_TYPE,
};

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
    static const uint8_t minAddress = 0x01;
    static const uint8_t maxAddress = 0xfd;
    static const uint8_t anonymousAddress = 0xfe;
    static const uint8_t broadcastAddress = 0xff;
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
    CanID(uint32_t id, uint32_t mask);
    CanID(const CanID &other);

    operator uint32_t() const;

    uint32_t getId() const;
    uint32_t getMask() const;

    CanID getReplyId() const;

    uint8_t getSource() const;
    CanID &setSource(uint8_t value);
    uint8_t getTarget() const;
    CanID &setTarget(uint8_t value);

    uint16_t getDataID() const;
    CanID &setDataID(uint16_t value);

    uint8_t getSequenceNumber() const;
    CanID &setSequenceNumber(uint8_t value);
    uint8_t getMessageNumber() const;
    CanID &setMessageNumber(uint8_t value);
    uint8_t getBridge() const;
    CanID &setBridge(uint8_t value);
    uint8_t getRandomElement() const;
    CanID &setRandomElement(uint8_t value);

    MessageType getMessageType() const;
    CanID &setMessageType(const MessageType value);

    MultiFrameMessageType getMultiFrameMessageType() const;
    CanID &setMultiFrameMessageType(const MultiFrameMessageType value);

    MessagePriority getMessagePriority() const;
    CanID &setMessagePriority(const MessagePriority value);

    static CanID create(uint32_t);
};

} // namespace ThingSet::Can