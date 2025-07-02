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
    virtual bool invoke(ThingSetDecoder &decoder, ThingSetEncoder &encoder) = 0;
};

template <typename Result, typename... Args>
static bool invoke(std::function<Result(Args...)> &function, std::tuple<Args...> &arguments,
                   ThingSetEncoder &encoder)
{
    Result result = std::apply(function, arguments);
    return encoder.encode(result);
}

template <typename... Args>
static bool invoke(std::function<void(Args...)> &function, std::tuple<Args...> &arguments,
                   ThingSetEncoder &encoder)
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
    template <unsigned ChildId, StringLiteral ArgName, typename T>
    class ThingSetFunctionParameter : public IdentifiableThingSetNode<ChildId, Id, ArgName>
    {
        const std::string getType() const override
        {
            return ThingSetType<T>::name.str();
        }

        constexpr ThingSetNodeType getNodeType() const override
        {
            return ThingSetNodeType::parameter;
        }

        constexpr ThingSetAccess getAccess() const override
        {
            return ThingSetAccess::anyReadWrite;
        }

        bool checkAccess(ThingSetAccess) const override
        {
            return true;
        }
    };

    // Inspired by https://stackoverflow.com/questions/67423250/transform-the-stdtuple-types-to-another-ones
    template<template<size_t> typename T, size_t Size, typename Sequence = std::make_index_sequence<Size>>
    struct _ArgumentTransformer;

    template<template<size_t> typename T, size_t... Indices>
    struct _ArgumentTransformer<T, sizeof...(Indices), std::index_sequence<Indices...>>
    {
        using type = std::tuple<typename T<Indices>::type...>;
    };

    template <size_t Index>
    struct _ParameterBuilder
    {
        using Tuple = std::tuple<Args...>;
        using ParameterType = std::tuple_element_t<Index, Tuple>;
        typedef ThingSetFunctionParameter<Id + 1 + Index, Name + ThingSetType<ParameterType>::name + "_" + to_string_t<1 + Index>(), ParameterType> type;
    };

    /// @brief The exposed function.
    std::function<Result(Args...)> _function;
    /// @brief A tuple containing ThingSet nodes which represent the parameters to the function.
    _ArgumentTransformer<_ParameterBuilder, sizeof...(Args)>::type _parameters;
    /// @brief The storage into which function arguments are decoded before invocation.
    std::tuple<Args...> _arguments;

public:
    ThingSetFunction(std::function<Result(Args...)> function)
        : IdentifiableThingSetParentNode<Id, ParentId, Name>::IdentifiableThingSetParentNode(), _function(function)
    {}

    const std::string getType() const override
    {
        return ThingSetType<std::function<Result(Args...)>>::name.str();
    }

    constexpr ThingSetNodeType getNodeType() const override
    {
        return ThingSetNodeType::function;
    }

    bool invoke(ThingSetDecoder &decoder, ThingSetEncoder &encoder) override
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