/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "thingset++/IdentifiableThingSetNode.hpp"
#include "thingset++/ThingSetCustomRequestHandler.hpp"
#include "thingset++/ThingSetType.hpp"
#include "thingset++/ThingSetValue.hpp"

namespace ThingSet {

template <typename T, typename Base>
concept IdentifiableBase = std::is_base_of_v<_IdentifiableThingSetNode<Base>, T>;

enum struct Subset : uint8_t
{
    persisted = 1 << 0,
    live = 1 << 1,
};

static constexpr Subset operator|(const Subset &lhs, const Subset &rhs)
{
    return (Subset)(((uint32_t)lhs) | ((uint32_t)rhs));
}

/// @brief @brief Encapsulates a ThingSet property, which is a value that can be read and/or written via the ThingSet API.
/// @tparam SubsetType Type of subset enumeration.
/// @tparam T Type of value.
/// @tparam NodeBase Ultimate base class.
/// @tparam Base Intermediate base class.
template <typename T, ThingSetAccess Access, typename SubsetType = Subset, SubsetType Subset = (SubsetType)0, NodeBase NodeBase = ThingSetNode, IdentifiableBase<NodeBase> Base = IdentifiableThingSetNode>
          requires std::is_enum_v<SubsetType>
class ThingSetProperty : public ThingSetValue<T>, public Base
{
public:
    ThingSetProperty(const uint16_t id, const uint16_t parentId, const std::string_view name) :
        ThingSetValue<T>(),
        Base(id, parentId, name)
    {}
    ThingSetProperty(const uint16_t id, const uint16_t parentId, const std::string_view name, const T &value) :
        ThingSetValue<T>(value),
        Base(id, parentId, name)
    {}

    using ThingSetValue<T>::operator=;

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

    uint32_t getSubsets() const override
    {
        return (uint32_t)Subset;
    }

    ThingSetAccess getAccess() const override
    {
        return Access;
    }

    bool checkAccess(ThingSetAccess access) const override
    {
        return (Access & access) == access;
    }
};

/// @brief Partial specialisation of ThingSetProperty for pointers to values.
template <typename T, ThingSetAccess Access, typename SubsetType, SubsetType Subset> requires std::is_enum_v<SubsetType>
class ThingSetProperty<T *, Access, SubsetType, Subset, ThingSetNode, IdentifiableThingSetNode>
    : public ThingSetValue<T *>, public IdentifiableThingSetNode
{
public:
    ThingSetProperty(const uint16_t id, const uint16_t parentId, const std::string_view name, T *value) :
        ThingSetValue<T *>(value),
        IdentifiableThingSetNode(id, parentId, name)
    {}

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

    uint32_t getSubsets() const override
    {
        return (uint32_t)Subset;
    }

    ThingSetAccess getAccess() const override
    {
        return Access;
    }

    bool checkAccess(ThingSetAccess access) const override
    {
        return (Access & access) == access;
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
};

/// @brief Partial specialisation of ThingSetProperty for record arrays.
template <typename Element, std::size_t Size, ThingSetAccess Access, typename SubsetType, SubsetType Subset>
    requires std::is_class_v<Element>
class ThingSetProperty<std::array<Element, Size>, Access, SubsetType, Subset, ThingSetParentNode, IdentifiableThingSetParentNode>
    : public ThingSetValue<std::array<Element, Size>>, public IdentifiableThingSetParentNode,
      public ThingSetCustomRequestHandler
{
public:
    ThingSetProperty(const uint16_t id, const uint16_t parentId, const std::string_view name) :
        ThingSetValue<std::array<Element, Size>>(),
        IdentifiableThingSetParentNode(id, parentId, name)
    {}
    ThingSetProperty(const uint16_t id, const uint16_t parentId, const std::string_view name, const std::array<Element, Size> &value) :
        ThingSetValue<std::array<Element, Size>>(value),
        IdentifiableThingSetParentNode(id, parentId, name)
    {}

    using ThingSetValue<std::array<Element, Size>>::operator=;

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
                return IdentifiableThingSetParentNode::tryCastTo(type, target);
        }
    }

    uint32_t getSubsets() const override
    {
        return (uint32_t)Subset;
    }

    ThingSetAccess getAccess() const override
    {
        return Access;
    }

    bool checkAccess(ThingSetAccess access) const override
    {
        return (Access & access) == access;
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
template <typename T, Subset S = (Subset)0>
using ThingSetReadOnlyProperty = ThingSetProperty<T, ThingSetAccess::anyRead, Subset, S>;

/// @brief A ThingSet property that can be read and written by anyone.
/// @tparam T The type of the value stored by this property.
/// @tparam Id The unique ID of the property.
/// @tparam ParentId The ID of the parent container of this property.
/// @tparam Name The human-readable name of the property.
template <typename T, Subset S = (Subset)0>
using ThingSetReadWriteProperty = ThingSetProperty<T, ThingSetAccess::anyReadWrite, Subset, S>;

/// @brief A ThingSet property that can be read by anyone but only written by advanced users.
/// @tparam T The type of the value stored by this property.
/// @tparam Id The unique ID of the property.
/// @tparam ParentId The ID of the parent container of this property.
/// @tparam Name The human-readable name of the property.
template <typename T, Subset S = (Subset)0>
using ThingSetReadAdvancedWriteProperty =
    ThingSetProperty<T, ThingSetAccess::anyRead | ThingSetAccess::expertWrite, Subset, S>;

/// @brief A ThingSet property that can be read by anyone but only written by the manufacturer.
/// @tparam T The type of the value stored by this property.
/// @tparam Id The unique ID of the property.
/// @tparam ParentId The ID of the parent container of this property.
/// @tparam Name The human-readable name of the property.
template <typename T, Subset S = (Subset)0>
using ThingSetReadManufacturerWriteProperty =
    ThingSetProperty<T, ThingSetAccess::anyRead | ThingSetAccess::manufacturerWrite, Subset, S>;

} // namespace ThingSet
