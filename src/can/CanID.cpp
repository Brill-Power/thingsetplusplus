/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "thingset++/can/CanID.hpp"

namespace ThingSet::Can {

#define CAN_FULL_MASK 0x1FFFFFFF

CanID::CanID() : _id(0), _mask(0)
{}

CanID::CanID(uint32_t id, uint32_t mask) : _id(id), _mask(mask)
{}

CanID::CanID(const CanID &other) : CanID(other._id, other._mask)
{}

CanID::operator uint32_t() const
{
    return _id;
}

CanID CanID::getReplyId() const
{
    CanID reply(*this);
    reply.setTarget(getSource()).setSource(getTarget());
    return reply;
}

uint32_t CanID::getId() const
{
    return _id;
}

uint32_t CanID::getMask() const
{
    return _mask;
}

uint8_t CanID::getSource() const
{
    return (_id & sourceMask) >> THINGSET_CAN_ID_POSITION_SOURCE;
}

CanID &CanID::setSource(uint8_t value)
{
    _id = (_id & ~sourceMask) | ((value << THINGSET_CAN_ID_POSITION_SOURCE) & sourceMask);
    _mask |= sourceMask;
    return *this;
}

uint8_t CanID::getTarget() const
{
    return (_id & targetMask) >> THINGSET_CAN_ID_POSITION_TARGET;
}

CanID &CanID::setTarget(uint8_t value)
{
    _id = (_id & ~targetMask) | ((value << THINGSET_CAN_ID_POSITION_TARGET) & targetMask);
    _mask |= targetMask;
    return *this;
}

uint16_t CanID::getDataID() const
{
    return (_id & dataIdMask) >> THINGSET_CAN_ID_POSITION_DATA_ID;
}

CanID &CanID::setDataID(uint16_t value)
{
    _id = (_id & ~dataIdMask) | ((value << THINGSET_CAN_ID_POSITION_DATA_ID) & dataIdMask);
    _mask |= dataIdMask;
    return *this;
}

uint8_t CanID::getSequenceNumber() const
{
    return (_id & sequenceNumberMask) >> THINGSET_CAN_ID_POSITION_SEQ_NO;
}

CanID &CanID::setSequenceNumber(uint8_t value)
{
    _id = (_id & ~sequenceNumberMask) | ((value << THINGSET_CAN_ID_POSITION_SEQ_NO) & sequenceNumberMask);
    _mask |= sequenceNumberMask;
    return *this;
}

uint8_t CanID::getMessageNumber() const
{
    return (_id & messageNumberMask) >> THINGSET_CAN_ID_POSITION_MSG_NO;
}

CanID &CanID::setMessageNumber(uint8_t value)
{
    _id = (_id & ~messageNumberMask) | ((value << THINGSET_CAN_ID_POSITION_MSG_NO) & messageNumberMask);
    _mask |= messageNumberMask;
    return *this;
}

uint8_t CanID::getBridge() const
{
    return (_id & bridgeMask) >> THINGSET_CAN_ID_POSITION_BRIDGE;
}

CanID &CanID::setBridge(uint8_t value)
{
    _id = (_id & ~bridgeMask) | ((value << THINGSET_CAN_ID_POSITION_BRIDGE) & bridgeMask);
    _mask |= bridgeMask;
    return *this;
}

uint8_t CanID::getRandomElement() const
{
    return getBridge();
}

CanID &CanID::setRandomElement(uint8_t value)
{
    return setBridge(value);
}

MessageType CanID::getMessageType() const
{
    return (MessageType)(_id & messageTypeMask);
}

CanID &CanID::setMessageType(const MessageType value)
{
    _id = (_id & ~messageTypeMask) | value;
    _mask |= messageTypeMask;
    return *this;
}

MultiFrameMessageType CanID::getMultiFrameMessageType() const
{
    return (MultiFrameMessageType)(_id & multiFrameTypeMask);
}

CanID &CanID::setMultiFrameMessageType(const MultiFrameMessageType value)
{
    _id = (_id & ~multiFrameTypeMask) | value;
    _mask |= multiFrameTypeMask;
    return *this;
}

MessagePriority CanID::getMessagePriority() const
{
    return (MessagePriority)(_id & messagePriorityMask);
}

CanID &CanID::setMessagePriority(const MessagePriority value)
{
    _id = (_id & ~messagePriorityMask) | value;
    _mask |= messagePriorityMask;
    return *this;
}

CanID CanID::create(uint32_t id)
{
    return CanID(id, CAN_FULL_MASK);
}

} // namespace ThingSet::Can