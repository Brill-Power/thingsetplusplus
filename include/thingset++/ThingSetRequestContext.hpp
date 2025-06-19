/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "ThingSetBinaryDecoder.hpp"
#include "ThingSetBinaryEncoder.hpp"
#include "ThingSetTextDecoder.hpp"
#include "ThingSetTextEncoder.hpp"
#include "ThingSetNode.hpp"
#include "ThingSetStatus.hpp"

namespace ThingSet {

/// @brief Context for a single ThingSet request.
class ThingSetRequestContext
{
protected:
    ThingSetRequestContext(uint8_t *resp, size_t responseLen);

public:
    /// @brief Pointer to the ThingSet node this request relates to.
    ThingSetNode *node;
    /// @brief If the node is an array, indicates the array element to which the request relates.
    size_t index;
    /// @brief Pointer to the buffer into which the response will be written.
    uint8_t *response;
    /// @brief If true, keys in responses will be encoded as integer IDs; if false, as strings.
    bool useIds;
    /// @brief The path to the node to which this request relates.
    std::string path;

    virtual ThingSetEncoder &encoder() = 0;
    virtual ThingSetDecoder &decoder() = 0;

    virtual bool isGet() = 0;
    virtual bool isFetch() = 0;
    virtual bool isUpdate() = 0;
    virtual bool isDelete() = 0;
    virtual bool isExec() = 0;
};

template <typename T>
concept Enum = std::is_enum_v<T>;

template <Enum RequestType>
class _ThingSetRequestContext : public ThingSetRequestContext
{
private:
    /// @brief Specifies the request type.
    RequestType _requestType;

protected:
    _ThingSetRequestContext(uint8_t *request, size_t requestLen, uint8_t *resp, size_t responseLen) :
        ThingSetRequestContext(resp, responseLen),
        _requestType((RequestType)request[0])
    {}

public:
    bool isGet() override {
        return _requestType == RequestType::get;
    }

    bool isFetch() override {
        return _requestType == RequestType::fetch;
    }

    bool isUpdate() override {
        return _requestType == RequestType::update;
    }

    bool isDelete() override {
        return _requestType == RequestType::remove;
    }

    bool isExec() override {
        return _requestType == RequestType::exec;
    }
};

class ThingSetBinaryRequestContext : public _ThingSetRequestContext<ThingSetBinaryRequestType>
{
private:
    DefaultFixedDepthThingSetBinaryEncoder _encoder;
    DefaultFixedDepthThingSetBinaryDecoder _decoder;

public:
    ThingSetBinaryRequestContext(uint8_t *request, size_t requestLen, uint8_t *resp, size_t responseLen);

    inline ThingSetEncoder &encoder() override {
        return _encoder;
    }

    inline ThingSetDecoder &decoder() override {
        return _decoder;
    }
};

class ThingSetTextRequestContext : public _ThingSetRequestContext<ThingSetTextRequestType>
{
private:
    ThingSetTextEncoder _encoder;
    DefaultFixedSizeThingSetTextDecoder _decoder;

public:
    ThingSetTextRequestContext(uint8_t *request, size_t requestLen, uint8_t *resp, size_t responseLen);

    inline ThingSetEncoder &encoder() override {
        return _encoder;
    }

    inline ThingSetDecoder &decoder() override {
        return _decoder;
    }
};

} // namespace ThingSet