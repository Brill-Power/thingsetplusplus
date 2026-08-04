/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "thingset++/IdentifiableThingSetNode.hpp"
#include "thingset++/ThingSetDecoder.hpp"
#include "thingset++/ThingSetEncoder.hpp"
#include "thingset++/ThingSetType.hpp"
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

template <typename Result, typename... Args, typename... Values>
static bool invoke(std::function<Result(Args...)> &function, std::tuple<Values...> &arguments,
                   ThingSetEncoder &encoder)
{
    Result result = std::apply(function, arguments);
    return encoder.encode(result);
}

template <typename... Args, typename... Values>
static bool invoke(std::function<void(Args...)> &function, std::tuple<Values...> &arguments,
                   ThingSetEncoder &encoder)
{
    std::apply(function, arguments);
    return encoder.encodeNull();
}

/// @brief Carries optional human-friendly names for a function's parameters.
/// Pass as the ParamNames argument of ThingSetFunction (or use one of the
/// ThingSetNamed*Function aliases). An empty list means every parameter
/// name is auto-generated from the function name, argument type and
/// position (e.g. "xOnu16_1"); a non-empty list must name every argument.
/// Names travel in the node metadata
template <StringLiteral... Names>
struct ThingSetParameterNames
{
    static constexpr size_t count = sizeof...(Names);

    template <StringLiteral Name, size_t Index, typename ParameterType>
    static constexpr auto get()
    {
        return std::get<Index>(std::tuple{ Names... });
    }
};

/// @brief Empty specialisation of ThingSetParameterNames for the
/// default case which auto-generates parameter names.
template <>
struct ThingSetParameterNames<>
{
    static constexpr size_t count = 0;

    template <StringLiteral Name, size_t Index, typename ParameterType>
    static constexpr auto get()
    {
        return Name + ThingSetType<std::remove_cvref_t<ParameterType>>::name + "_" + to_string_t<1 + Index>();
    }
};

/// @brief Represents an executable function.
/// @tparam Id The unique integer ID of the ThingSet node.
/// @tparam ParentId The integer ID of the parent node.
/// @tparam Name The name of the node.
/// @tparam Access Access control flags.
/// @tparam ParamNames A ThingSetParameterNames<...> naming each argument,
/// or ThingSetParameterNames<> to auto-generate names.
/// @tparam Result The return type of the function.
/// @tparam ...Args The argument types of the function, if any.
template <uint16_t Id, uint16_t ParentId, StringLiteral Name, ThingSetAccess Access, uint16_t FirstArgumentId, typename ParamNames, typename Result, typename... Args>
    requires (ParamNames::count == 0 || ParamNames::count == sizeof...(Args))
class ThingSetFunction : public IdentifiableThingSetParentNode<Id, ParentId, Name>, public ThingSetInvocable
{
private:
    template <uint16_t ChildId, StringLiteral ArgName, typename T>
    class ThingSetFunctionParameter : public IdentifiableThingSetNode<ChildId, Id, ArgName>
    {
    public:
        ThingSetFunctionParameter() : IdentifiableThingSetNode<ChildId, Id, ArgName>()
        {}

        const std::string getType() const override
        {
            return ThingSetType<std::remove_cvref_t<T>>::name.str();
        }

        constexpr ThingSetAccess getAccess() const override
        {
            return ThingSetAccess::anyReadWrite;
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

        static constexpr auto argName()
        {
            return ParamNames::template get<Name, Index, ParameterType>();
        }

        typedef ThingSetFunctionParameter<FirstArgumentId + 0 + Index, argName(), ParameterType> type;
    };

    /// @brief The exposed function.
    std::function<Result(Args...)> _function;
    /// @brief A tuple containing ThingSet nodes which represent the parameters to the function.
    _ArgumentTransformer<_ParameterBuilder, sizeof...(Args)>::type _parameters;
    /// @brief The storage into which function arguments are decoded before invocation.
    std::tuple<std::remove_cvref_t<Args>...> _arguments;

public:
    ThingSetFunction(std::function<Result(Args...)> function)
        : IdentifiableThingSetParentNode<Id, ParentId, Name>(), _function(function)
    {}

    constexpr const std::string getType() const override
    {
        return ThingSetType<std::function<Result(std::remove_cvref_t<Args>...)>>::name.str();
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

    constexpr ThingSetAccess getAccess() const override
    {
        return Access;
    }

    bool invokeCallback(ThingSetNode *, ThingSetCallbackReason) const override
    {
        return true;
    }
};

template <uint16_t Id, uint16_t ParentId, StringLiteral Name, typename Result, typename... Args>
using ThingSetUserFunction =
    ThingSetFunction<Id, ParentId, Name, ThingSetAccess::anyWrite, Id + 1, ThingSetParameterNames<>, Result, Args...>;

template <uint16_t Id, uint16_t ParentId, StringLiteral Name, typename Result, typename... Args>
using ThingSetAdvancedFunction =
    ThingSetFunction<Id, ParentId, Name, ThingSetAccess::expertWrite, Id + 1, ThingSetParameterNames<>, Result, Args...>;

template <uint16_t Id, uint16_t ParentId, StringLiteral Name, typename Result, typename... Args>
using ThingSetManufacturerFunction =
    ThingSetFunction<Id, ParentId, Name, ThingSetAccess::manufacturerWrite, Id + 1, ThingSetParameterNames<>, Result,
                     Args...>;

// Named variants: identical to the above but with human-friendly parameter
// names, e.g.
//   ThingSetNamedUserFunction<0x710, 0x07, "xOn", ThingSetParameterNames<"uSwitchMode">, int, uint16_t>
template <uint16_t Id, uint16_t ParentId, StringLiteral Name, typename ParamNames, typename Result, typename... Args>
using ThingSetNamedUserFunction =
    ThingSetFunction<Id, ParentId, Name, ThingSetAccess::anyWrite, Id + 1, ParamNames, Result, Args...>;

template <uint16_t Id, uint16_t ParentId, StringLiteral Name, typename ParamNames, typename Result, typename... Args>
using ThingSetNamedAdvancedFunction =
    ThingSetFunction<Id, ParentId, Name, ThingSetAccess::expertWrite, Id + 1, ParamNames, Result, Args...>;

template <uint16_t Id, uint16_t ParentId, StringLiteral Name, typename ParamNames, typename Result, typename... Args>
using ThingSetNamedManufacturerFunction =
    ThingSetFunction<Id, ParentId, Name, ThingSetAccess::manufacturerWrite, Id + 1, ParamNames, Result, Args...>;

} // namespace ThingSet