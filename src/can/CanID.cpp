/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "thingset++/can/CanID.hpp"

namespace ThingSet::Can {

#define CAN_SFF_MASK 0x000007FF
#define CAN_EFF_MASK 0x1FFFFFFF
#define CAN_EFF_FLAG 0x80000000

CanID::CanID() : _id(0), _mask(0)
{}

CanID::CanID(const uint32_t id, const uint32_t mask) : _id(id), _mask(mask)
{}

CanID::CanID(const CanID &other) : CanID(other._id, other._mask)
{}

CanID::operator uint32_t() const
{
    return getId();
}

CanID CanID::getReplyId() const
{
    CanID reply(*this);
    reply.setTarget(getSource()).setSource(getTarget());
    return reply;
}

uint32_t CanID::getIdWithFlags() const
{
    return _id > CAN_SFF_MASK ? (_id | CAN_EFF_FLAG) : _id;
}

uint32_t CanID::getId() const
{
    return _id;
}

uint32_t CanID::getMask() const
{
    return _mask;
}

CanID &CanID::setMask(const uint32_t value)
{
    _mask = value;
    return *this;
}

uint8_t CanID::getSource() const
{
    return (_id & sourceMask) >> THINGSET_PLUS_PLUS_CAN_ID_POSITION_SOURCE;
}

CanID &CanID::setSource(const uint8_t value)
{
    _id = (_id & ~sourceMask) | ((value << THINGSET_PLUS_PLUS_CAN_ID_POSITION_SOURCE) & sourceMask);
    _mask |= sourceMask;
    return *this;
}

uint8_t CanID::getTarget() const
{
    return (_id & targetMask) >> THINGSET_PLUS_PLUS_CAN_ID_POSITION_TARGET;
}

CanID &CanID::setTarget(const uint8_t value)
{
    _id = (_id & ~targetMask) | ((value << THINGSET_PLUS_PLUS_CAN_ID_POSITION_TARGET) & targetMask);
    _mask |= targetMask;
    return *this;
}

uint16_t CanID::getDataId() const
{
    return (_id & dataIdMask) >> THINGSET_PLUS_PLUS_CAN_ID_POSITION_DATA_ID;
}

CanID &CanID::setDataId(const uint16_t value)
{
    _id = (_id & ~dataIdMask) | ((value << THINGSET_PLUS_PLUS_CAN_ID_POSITION_DATA_ID) & dataIdMask);
    _mask |= dataIdMask;
    return *this;
}

uint8_t CanID::getSequenceNumber() const
{
    return (_id & sequenceNumberMask) >> THINGSET_PLUS_PLUS_CAN_ID_POSITION_SEQ_NO;
}

CanID &CanID::setSequenceNumber(const uint8_t value)
{
    _id = (_id & ~sequenceNumberMask) | ((value << THINGSET_PLUS_PLUS_CAN_ID_POSITION_SEQ_NO) & sequenceNumberMask);
    _mask |= sequenceNumberMask;
    return *this;
}

uint8_t CanID::getMessageNumber() const
{
    return (_id & messageNumberMask) >> THINGSET_PLUS_PLUS_CAN_ID_POSITION_MSG_NO;
}

CanID &CanID::setMessageNumber(const uint8_t value)
{
    _id = (_id & ~messageNumberMask) | ((value << THINGSET_PLUS_PLUS_CAN_ID_POSITION_MSG_NO) & messageNumberMask);
    _mask |= messageNumberMask;
    return *this;
}

uint8_t CanID::getBridge() const
{
    return (_id & bridgeMask) >> THINGSET_PLUS_PLUS_CAN_ID_POSITION_BRIDGE;
}

CanID &CanID::setBridge(const uint8_t value)
{
    _id = (_id & ~bridgeMask) | ((value << THINGSET_PLUS_PLUS_CAN_ID_POSITION_BRIDGE) & bridgeMask);
    _mask |= bridgeMask;
    return *this;
}

uint8_t CanID::getRandomElement() const
{
    return getBridge();
}

CanID &CanID::setRandomElement(const uint8_t value)
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

CanID CanID::create(const uint32_t id)
{
    uint32_t mask = id <= CAN_SFF_MASK ? CAN_SFF_MASK : CAN_EFF_MASK;
    return CanID(id & mask, mask);
}

#ifndef __ZEPHYR__
std::ostream& operator<<(std::ostream &os, const CanID &id)
{
    os << "0x" << std::hex << id.getId();
    return os;
}
#endif // __ZEPHYR__

} // namespace ThingSet::Can