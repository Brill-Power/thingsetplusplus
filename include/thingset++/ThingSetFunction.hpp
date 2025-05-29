/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "IdentifiableThingSetNode.hpp"
#include "StringLiteral.hpp"
#include "ThingSetBinaryDecoder.hpp"
#include "ThingSetBinaryEncoder.hpp"
#include "ThingSetType.hpp"
#include <concepts>
#include <functional>
#include <tuple>

namespace ThingSet {

/// @brief Interface for executable functions.
class ThingSetInvocable
{
public:
    virtual bool invoke(ThingSetBinaryDecoder &decoder, ThingSetBinaryEncoder &encoder) = 0;
};

template <typename Result, typename... Args>
static bool invoke(std::function<Result(Args...)> &function, std::tuple<Args...> &arguments,
                   ThingSetBinaryEncoder &encoder)
{
    Result result = std::apply(function, arguments);
    return encoder.encode(result);
}

template <typename... Args>
static bool invoke(std::function<void(Args...)> &function, std::tuple<Args...> &arguments,
                   ThingSetBinaryEncoder &encoder)
{
    std::apply(function, arguments);
    return encoder.encodeNull();
}

/// @brief Represents an executable function.
/// @tparam Id The unique integer ID of the ThingSet node.
/// @tparam ParentId The integer ID of the parent node.
/// @tparam Name The name of the node.
/// @tparam Access Access control flags.
/// @tparam Result The return type of the function.
/// @tparam ...Args The argument types of the function, if any.
template <unsigned Id, unsigned ParentId, StringLiteral Name, ThingSetAccess Access, typename Result, typename... Args>
class ThingSetFunction : public IdentifiableThingSetParentNode<Id, ParentId, Name>, public ThingSetInvocable
{
private:
    /// @brief The exposed function.
    std::function<Result(Args...)> _function;
    /// @brief The storage into which function arguments are decoded before invocation.
    std::tuple<Args...> _arguments;

public:
    ThingSetFunction(std::function<Result(Args...)> function)
        : IdentifiableThingSetParentNode<Id, ParentId, Name>::IdentifiableThingSetParentNode(), _function(function)
    {}

    const std::string getType() const override
    {
        return ThingSetType<std::function<Result(Args...)>>::name;
    }

    constexpr ThingSetNodeType getNodeType() const override
    {
        return ThingSetNodeType::function;
    }

    bool invoke(ThingSetBinaryDecoder &decoder, ThingSetBinaryEncoder &encoder) override
    {
        return decoder.decodeList(_arguments) && ThingSet::invoke(_function, _arguments, encoder);
    }

    bool tryCastTo(ThingSetNodeType type, void **target) override
    {
        switch (type) {
            case ThingSetNodeType::function:
                *target = static_cast<ThingSetInvocable *>(this);
                return true;
            case ThingSetNodeType::hasChildren:
                *target = static_cast<ThingSetParentNode *>(this);
                return true;
            default:
                return false;
        }
    }

    ThingSetAccess getAccess() const override
    {
        return Access;
    }

    bool checkAccess(ThingSetAccess access) const override
    {
        return (access & Access) == Access;
    }

    bool invokeCallback(ThingSetNode *, ThingSetCallbackReason) const override
    {
        return true;
    }
};

template <unsigned Id, unsigned ParentId, StringLiteral Name, typename Result, typename... Args>
using ThingSetUserFunction = ThingSetFunction<Id, ParentId, Name, ThingSetAccess::anyWrite, Result, Args...>;

template <unsigned Id, unsigned ParentId, StringLiteral Name, typename Result, typename... Args>
using ThingSetAdvancedFunction = ThingSetFunction<Id, ParentId, Name, ThingSetAccess::expertWrite, Result, Args...>;

template <unsigned Id, unsigned ParentId, StringLiteral Name, typename Result, typename... Args>
using ThingSetManufacturerFunction =
    ThingSetFunction<Id, ParentId, Name, ThingSetAccess::manufacturerWrite, Result, Args...>;

} // namespace ThingSet