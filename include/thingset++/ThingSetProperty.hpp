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

template <typename T, typename Base, unsigned Id, unsigned ParentId, StringLiteral Name>
concept IdentifiableBase = std::is_base_of_v<_IdentifiableThingSetNode<Base, Id, ParentId, Name>, T>;

enum struct Subset
{
    persisted = 1 << 0,
    live = 1 << 1,
};

static constexpr Subset operator|(const Subset &lhs, const Subset &rhs)
{
    return (Subset)(((uint32_t)lhs) | ((uint32_t)rhs));
}

template <unsigned Id, unsigned ParentId, StringLiteral Name, NodeBase NodeBase,
          IdentifiableBase<NodeBase, Id, ParentId, Name> Base, ThingSetAccess Access, typename T, typename SubsetType = Subset, SubsetType Subset = (SubsetType)0>
          requires std::is_enum_v<SubsetType>
class _ThingSetProperty : public ThingSetValue<T>, public Base
{
protected:
    _ThingSetProperty() : ThingSetValue<T>(), Base()
    {}
    _ThingSetProperty(const T &value) : ThingSetValue<T>(value), Base()
    {}

public:
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
};

/// @brief Encapsulates a ThingSet property, which is a value that can be read and/or written via the ThingSet API.
/// @tparam T The type of the value stored by this property.
/// @tparam Id The unique ID of the property.
/// @tparam ParentId The ID of the parent container of this property.
/// @tparam Name The human-readable name of the property.
/// @tparam Access The access permissions for this property.
template <unsigned Id, unsigned ParentId, StringLiteral Name, ThingSetAccess Access, typename T, typename SubsetType = Subset, SubsetType Subset = (SubsetType)0>
    requires std::is_enum_v<SubsetType>
class ThingSetProperty : public _ThingSetProperty<Id, ParentId, Name, ThingSetNode,
                                                  IdentifiableThingSetNode<Id, ParentId, Name>, Access, T, SubsetType, Subset>
{
public:
    ThingSetProperty()
        : _ThingSetProperty<Id, ParentId, Name, ThingSetNode, IdentifiableThingSetNode<Id, ParentId, Name>, Access, T, SubsetType, Subset>()
    {}
    ThingSetProperty(const T &value)
        : _ThingSetProperty<Id, ParentId, Name, ThingSetNode, IdentifiableThingSetNode<Id, ParentId, Name>, Access, T, SubsetType, Subset>(
              value)
    {}

    auto &operator=(const T &value)
    {
        this->_value = value;
        return *this;
    }

    auto &operator=(T &&value)
    {
        this->_value = std::move(value);
        return *this;
    }
};

/// @brief Partial specialisation of ThingSetProperty for pointers to values.
template <unsigned Id, unsigned ParentId, StringLiteral Name, ThingSetAccess Access, typename T, typename SubsetType, SubsetType Subset>
class ThingSetProperty<Id, ParentId, Name, Access, T *, SubsetType, Subset>
    : public _ThingSetProperty<Id, ParentId, Name, ThingSetNode, IdentifiableThingSetNode<Id, ParentId, Name>, Access, T *, SubsetType, Subset>
{
public:
    ThingSetProperty(T *value)
        : _ThingSetProperty<Id, ParentId, Name, ThingSetNode, IdentifiableThingSetNode<Id, ParentId, Name>, Access, T *, SubsetType, Subset>(value)
    {}

    auto &operator=(T &value)
    {
        *this->_value = value;
    }

    auto &operator=(T &&value)
    {
        *this->_value = std::move(value);
        return *this;
    }
};

/// @brief Partial specialisation of ThingSetProperty for record arrays.
template <unsigned Id, unsigned ParentId, StringLiteral Name, ThingSetAccess Access, typename Element, std::size_t Size, typename SubsetType, SubsetType Subset>
    requires std::is_class_v<Element>
class ThingSetProperty<Id, ParentId, Name, Access, std::array<Element, Size>, SubsetType, Subset>
    : public _ThingSetProperty<Id, ParentId, Name, ThingSetParentNode,
                               IdentifiableThingSetParentNode<Id, ParentId, Name>, Access, std::array<Element, Size>, SubsetType, Subset>,
      public ThingSetCustomRequestHandler
{
public:
    ThingSetProperty()
        : _ThingSetProperty<Id, ParentId, Name, ThingSetParentNode, IdentifiableThingSetParentNode<Id, ParentId, Name>,
                            Access, std::array<Element, Size>, SubsetType, Subset>()
    {}
    ThingSetProperty(const std::array<Element, Size> &value)
        : _ThingSetProperty<Id, ParentId, Name, ThingSetParentNode, IdentifiableThingSetParentNode<Id, ParentId, Name>,
                            Access, std::array<Element, Size>, SubsetType, Subset>(value)
    {}

    auto &operator=(const std::array<Element, Size> &value)
    {
        this->_value = value;
        return *this;
    }

    auto &operator=(std::array<Element, Size> &&value)
    {
        this->_value = std::move(value);
        return *this;
    }

    ThingSetParentNode::ChildIterator begin() override
    {
        // don't expose child nodes
        return this->end();
    }

    bool tryCastTo(ThingSetNodeType type, void **target) override
    {
        if (!_ThingSetProperty<Id, ParentId, Name, ThingSetParentNode,
                               IdentifiableThingSetParentNode<Id, ParentId, Name>, Access,
                               std::array<Element, Size>, SubsetType, Subset>::tryCastTo(type, target))
        {
            if (type == ThingSetNodeType::requestHandler) {
                *target = static_cast<ThingSetCustomRequestHandler *>(this);
                return true;
            }

            return false;
        }
        return true;
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
template <unsigned Id, unsigned ParentId, StringLiteral Name, typename T, Subset S = (Subset)0>
using ThingSetReadOnlyProperty = ThingSetProperty<Id, ParentId, Name, ThingSetAccess::anyRead, T, Subset, S>;

/// @brief A ThingSet property that can be read and written by anyone.
/// @tparam T The type of the value stored by this property.
/// @tparam Id The unique ID of the property.
/// @tparam ParentId The ID of the parent container of this property.
/// @tparam Name The human-readable name of the property.
template <unsigned Id, unsigned ParentId, StringLiteral Name, typename T, Subset S = (Subset)0>
using ThingSetReadWriteProperty = ThingSetProperty<Id, ParentId, Name, ThingSetAccess::anyReadWrite, T, Subset, S>;

/// @brief A ThingSet property that can be read by anyone but only written by advanced users.
/// @tparam T The type of the value stored by this property.
/// @tparam Id The unique ID of the property.
/// @tparam ParentId The ID of the parent container of this property.
/// @tparam Name The human-readable name of the property.
template <unsigned Id, unsigned ParentId, StringLiteral Name, typename T, Subset S = (Subset)0>
using ThingSetReadAdvancedWriteProperty =
    ThingSetProperty<Id, ParentId, Name, ThingSetAccess::anyRead | ThingSetAccess::expertWrite, T, Subset, S>;

/// @brief A ThingSet property that can be read by anyone but only written by the manufacturer.
/// @tparam T The type of the value stored by this property.
/// @tparam Id The unique ID of the property.
/// @tparam ParentId The ID of the parent container of this property.
/// @tparam Name The human-readable name of the property.
template <unsigned Id, unsigned ParentId, StringLiteral Name, typename T, Subset S = (Subset)0>
using ThingSetReadManufacturerWriteProperty =
    ThingSetProperty<Id, ParentId, Name, ThingSetAccess::anyRead | ThingSetAccess::expertWrite, T, Subset, S>;

} // namespace ThingSet
