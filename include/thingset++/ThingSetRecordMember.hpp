/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "thingset++/ThingSetProperty.hpp"

namespace ThingSet {

/// @brief Encapsulates a ThingSet property, which is a value that can be read and/or written via the ThingSet API.
/// @tparam T The type of the value stored by this property.
/// @tparam Id The unique ID of the property.
/// @tparam ParentId The ID of the parent container of this property.
/// @tparam Name The human-readable name of the property.
/// @tparam Access The access permissions for this property.
template <uint16_t Id, uint16_t ParentId, StringLiteral Name, ThingSetAccess Access, typename T, typename SubsetType = Subset,
          SubsetType Subset = (SubsetType)0, NodeBase Base = ThingSetNode>
          requires std::is_enum_v<SubsetType>
class ThingSetRecordMember : public ThingSetValue<T>, public Base
{
public:
    ThingSetRecordMember() : ThingSetValue<T>(), Base()
    {
        ThingSetRegistry::registerNode(this);
    }

    ThingSetRecordMember(const T &value) : ThingSetValue<T>(value), Base()
    {
        ThingSetRegistry::registerNode(this);
    }

    ~ThingSetRecordMember()
    {
        ThingSetRegistry::unregisterNode(this);
    }

    using ThingSetValue<T>::operator=;

    constexpr const std::string_view getName() const override
    {
        return Name.string_view();
    }

    constexpr unsigned getId() const override
    {
        return Id;
    }

    constexpr unsigned getParentId() const override
    {
        return ParentId;
    }

    constexpr const std::string getType() const override
    {
        return ThingSetType<T>::name.str();
    }

    constexpr ThingSetNodeType getNodeType() const override
    {
        return ThingSetNodeType::value;
    }

    bool tryCastTo(ThingSetNodeType type, void **target) override
    {
        switch (type) {
            case ThingSetNodeType::encodable:
                *target = static_cast<ThingSetEncodable *>(this);
                return true;
            case ThingSetNodeType::decodable:
                *target = static_cast<ThingSetDecodable *>(this);
                return true;
            default:
                return Base::tryCastTo(type, target);
        }
    }

    constexpr uint32_t getSubsets() const override
    {
        return (uint32_t)Subset;
    }

    constexpr ThingSetAccess getAccess() const override
    {
        return Access;
    }

    bool checkAccess(ThingSetAccess access) const override
    {
        return (Access & access) == Access;
    }

    constexpr static const unsigned id = Id;
    constexpr static const std::string_view &name = Name.string_view();
};

/// @brief Partial specialisation of ThingSetRecordMember for pointers to values.
template <uint16_t Id, uint16_t ParentId, StringLiteral Name, ThingSetAccess Access, typename T, typename SubsetType, SubsetType Subset>
class ThingSetRecordMember<Id, ParentId, Name, Access, T *, SubsetType, Subset, ThingSetNode>
    : public ThingSetValue<T *>, public ThingSetNode
{
public:
    ThingSetRecordMember(T *value) : ThingSetValue<T *>(value), ThingSetNode()
    {
        ThingSetRegistry::registerNode(this);
    }

    ~ThingSetRecordMember()
    {
        ThingSetRegistry::unregisterNode(this);
    }

    constexpr const std::string_view getName() const override
    {
        return Name.string_view();
    }

    constexpr unsigned getId() const override
    {
        return Id;
    }

    constexpr unsigned getParentId() const override
    {
        return ParentId;
    }

    constexpr const std::string getType() const override
    {
        return ThingSetType<std::remove_pointer_t<T>>::name.str();
    }

    constexpr ThingSetNodeType getNodeType() const override
    {
        return ThingSetNodeType::value;
    }

    bool tryCastTo(ThingSetNodeType type, void **target) override
    {
        switch (type) {
            case ThingSetNodeType::encodable:
                *target = static_cast<ThingSetEncodable *>(this);
                return true;
            case ThingSetNodeType::decodable:
                *target = static_cast<ThingSetDecodable *>(this);
                return true;
            default:
                return IdentifiableThingSetNode::tryCastTo(type, target);
        }
    }

    constexpr uint32_t getSubsets() const override
    {
        return (uint32_t)Subset;
    }

    constexpr ThingSetAccess getAccess() const override
    {
        return Access;
    }

    bool checkAccess(ThingSetAccess access) const override
    {
        return (Access & access) == Access;
    }

    auto &operator=(T &value)
    {
        *this->_value = value;
        return *this;
    }

    auto &operator=(T &&value)
    {
        *this->_value = std::move(value);
        return *this;
    }

    constexpr static const uint16_t id = Id;
    constexpr static const std::string_view &name = Name.string_view();
};

/// @brief Partial specialisation of ThingSetRecordMember for record arrays.
template <uint16_t Id, uint16_t ParentId, StringLiteral Name, ThingSetAccess Access, typename Element, std::size_t Size, typename SubsetType, SubsetType Subset>
    requires std::is_class_v<Element>
class ThingSetRecordMember<Id, ParentId, Name, Access, std::array<Element, Size>, SubsetType, Subset, ThingSetParentNode>
    : public ThingSetValue<std::array<Element, Size>>, public ThingSetParentNode,
      public ThingSetCustomRequestHandler
{
public:
    ThingSetRecordMember() : ThingSetValue<std::array<Element, Size>>(), ThingSetParentNode()
    {
        ThingSetRegistry::registerNode(this);
    }

    ThingSetRecordMember(const std::array<Element, Size> &value) : ThingSetValue<std::array<Element, Size>>(value),
        ThingSetParentNode()
    {
        ThingSetRegistry::registerNode(this);
    }

    ~ThingSetRecordMember()
    {
        ThingSetRegistry::unregisterNode(this);
    }

    using ThingSetValue<std::array<Element, Size>>::operator=;

    constexpr const std::string_view getName() const override
    {
        return Name.string_view();
    }

    constexpr unsigned getId() const override
    {
        return Id;
    }

    constexpr unsigned getParentId() const override
    {
        return ParentId;
    }

    ThingSetParentNode::ChildIterator begin() override
    {
        // don't expose child nodes
        return this->end();
    }

    constexpr const std::string getType() const override
    {
        return ThingSetType<std::array<Element, Size>>::name.str();
    }

    constexpr ThingSetNodeType getNodeType() const override
    {
        return ThingSetNodeType::value;
    }

    bool tryCastTo(ThingSetNodeType type, void **target) override
    {
        switch (type) {
            case ThingSetNodeType::encodable:
                *target = static_cast<ThingSetEncodable *>(this);
                return true;
            case ThingSetNodeType::decodable:
                *target = static_cast<ThingSetDecodable *>(this);
                return true;
            case ThingSetNodeType::requestHandler:
                *target = static_cast<ThingSetCustomRequestHandler *>(this);
                return true;
            default:
                return ThingSetParentNode::tryCastTo(type, target);
        }
    }

    constexpr uint32_t getSubsets() const override
    {
        return (uint32_t)Subset;
    }

    constexpr ThingSetAccess getAccess() const override
    {
        return Access;
    }

    bool checkAccess(ThingSetAccess access) const override
    {
        return (Access & access) == Access;
    }

    bool findByName(const std::string &name, ThingSetNode **node, size_t *index) override
    {
        bool foundChild = ThingSetParentNode::findByName(name, node, index);
        // atoi is useless - it returns 0 on failure - so we check that the string actually
        // contains a number before trying to parse it
        if (name.c_str()[0] >= '0' && name.c_str()[0] <= '9') {
            *index = std::atoi(name.c_str());
            return true;
        }

        return foundChild;
    }

    bool invokeCallback(ThingSetNode *, ThingSetCallbackReason) const override
    {
        return true;
    }

    int handleRequest(ThingSetRequestContext &context) override
    {
        if (context.isGet()) {
            context.setStatus(ThingSetStatusCode::content);
            context.encoder().encodePreamble();
            if (context.index == SIZE_MAX) {
                context.encoder().encode(
#if defined(__APPLE__) || defined(__OpenBSD__)
                    // working round ambiguity on macOS and OpenBSD
                    // https://stackoverflow.com/questions/42004974/function-overloading-integer-types-and-stdsize-t-on-64-bit-systems
                    static_cast<uint32_t>(
#endif
                        this->_value.size()
#if defined(__APPLE__) || defined(__OpenBSD__)
                    )
#endif
                );
            }
            else {
                context.encoder().encode(this->_value[context.index]);
            }
            return context.getHeaderLength() + context.encoder().getEncodedLength();
        }
        else if (context.isUpdate()) {
            context.setStatus(ThingSetStatusCode::changed);
            context.encoder().encodePreamble();
            if (context.index == SIZE_MAX) {
                context.setStatus(ThingSetStatusCode::badRequest);
            }
            context.decoder().decode(&this->_value[context.index]);
            return context.getHeaderLength();
        }
        return 0;
    }
};

/// @brief A ThingSet property that can be read by anyone.
/// @tparam T The type of the value stored by this property.
/// @tparam Id The unique ID of the property.
/// @tparam ParentId The ID of the parent container of this property.
/// @tparam Name The human-readable name of the property.
template <uint16_t Id, uint16_t ParentId, StringLiteral Name, typename T, Subset S = (Subset)0>
using ThingSetReadOnlyRecordMember = ThingSetRecordMember<Id, ParentId, Name, ThingSetAccess::anyRead, T, Subset, S>;

/// @brief A ThingSet property that can be read and written by anyone.
/// @tparam T The type of the value stored by this property.
/// @tparam Id The unique ID of the property.
/// @tparam ParentId The ID of the parent container of this property.
/// @tparam Name The human-readable name of the property.
template <uint16_t Id, uint16_t ParentId, StringLiteral Name, typename T, Subset S = (Subset)0>
using ThingSetReadWriteRecordMember = ThingSetRecordMember<Id, ParentId, Name, ThingSetAccess::anyReadWrite, T, Subset, S>;

/// @brief A ThingSet property that can be read by anyone but only written by advanced users.
/// @tparam T The type of the value stored by this property.
/// @tparam Id The unique ID of the property.
/// @tparam ParentId The ID of the parent container of this property.
/// @tparam Name The human-readable name of the property.
template <uint16_t Id, uint16_t ParentId, StringLiteral Name, typename T, Subset S = (Subset)0>
using ThingSetReadAdvancedWriteRecordMember =
    ThingSetRecordMember<Id, ParentId, Name, ThingSetAccess::anyRead | ThingSetAccess::expertWrite, T, Subset, S>;

/// @brief A ThingSet property that can be read by anyone but only written by the manufacturer.
/// @tparam T The type of the value stored by this property.
/// @tparam Id The unique ID of the property.
/// @tparam ParentId The ID of the parent container of this property.
/// @tparam Name The human-readable name of the property.
template <uint16_t Id, uint16_t ParentId, StringLiteral Name, typename T, Subset S = (Subset)0>
using ThingSetReadManufacturerWriteRecordMember =
    ThingSetRecordMember<Id, ParentId, Name, ThingSetAccess::anyRead | ThingSetAccess::expertWrite, T, Subset, S>;


} // namespace ThingSet