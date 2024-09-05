/*
 * Copyright (c) 2024 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
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
/// @tparam id The unique integer ID of the ThingSet node.
/// @tparam parentId The integer ID of the parent node.
/// @tparam name The name of the node.
/// @tparam Result The return type of the function.
/// @tparam ...Args The argument types of the function, if any.
template <unsigned id, unsigned parentId, StringLiteral name, typename Result, typename... Args>
class ThingSetFunction : public IdentifiableThingSetNode<id, parentId, name>,
                         public ThingSetParentNode,
                         public ThingSetInvocable
{
private:
    /// @brief The exposed function.
    std::function<Result(Args...)> _function;
    /// @brief The storage into which function arguments are decoded before invocation.
    std::tuple<Args...> _arguments;

public:
    ThingSetFunction(std::function<Result(Args...)> function) : _function(function)
    {}

    const std::string getType() const override
    {
        return ThingSetType<std::function<Result(Args...)>>::name;
    }

    constexpr const ThingSetNodeType getNodeType() const override
    {
        return ThingSetNodeType::function;
    }

    bool invoke(ThingSetBinaryDecoder &decoder, ThingSetBinaryEncoder &encoder) override
    {
        return decoder.decodeList(_arguments) && ThingSet::invoke(_function, _arguments, encoder);
    }
};

} // namespace ThingSet