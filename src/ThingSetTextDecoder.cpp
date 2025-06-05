/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "thingset++/ThingSetTextDecoder.hpp"

namespace ThingSet {

bool ThingSetTextDecoder::getIsForwardOnly() const
{
    return false;
}

bool ThingSetTextDecoder::decode(std::string *value)
{
    int endIndex = _bufferElemPtr;

    if (_inputBuffer[_bufferElemPtr] != '\"') {
        return false;
    }

    // make sure \" is ignored
    _bufferElemPtr++;
    endIndex++;

    while ((_bufferElemPtr < _bufferSize) && (_inputBuffer[endIndex] != '\"')) {
        endIndex++;
    }

    if (_inputBuffer[endIndex] != '\"') {
        return false;
    }

    *value = std::string(&_inputBuffer[_bufferElemPtr], endIndex - _bufferElemPtr);
    _bufferElemPtr = endIndex + 1; // + 1 to ignore closing \"
    return true;
}

bool ThingSetTextDecoder::decode(char *value)
{
    *value = _inputBuffer[_bufferElemPtr];
    _bufferElemPtr++;
    return true;
}

bool ThingSetTextDecoder::decode(float *value)
{
    auto decodeFunction = [](const char *start, char **end) -> float {
        return static_cast<float>(std::strtod(start, end));
    };
    return decodeValue<float>(value, decodeFunction);
}

bool ThingSetTextDecoder::decode(double *value)
{
    return decodeValue(value, std::strtod);
}

bool ThingSetTextDecoder::decode(bool *value)
{
    uint8_t retVal;

    if (!decode(&retVal)) {
        return false;
    }

    if (retVal > 1) {
        return false;
    }

    *value = static_cast<bool>(retVal);

    return true;
}

bool ThingSetTextDecoder::decode(uint8_t *value)
{
    auto decodeFunction = [](const char *start, char **end) -> uint8_t {
        return static_cast<uint8_t>(std::strtoul(start, end, 10));
    };
    return decodeValue<uint8_t>(value, decodeFunction);
}

bool ThingSetTextDecoder::decode(uint16_t *value)
{
    auto decodeFunction = [](const char *start, char **end) -> uint16_t {
        return static_cast<uint16_t>(std::strtoul(start, end, 10));
    };
    return decodeValue<uint16_t>(value, decodeFunction);
}

bool ThingSetTextDecoder::decode(uint32_t *value)
{
    auto decodeFunction = [](const char *start, char **end) -> uint32_t {
        return static_cast<uint32_t>(std::strtoul(start, end, 10));
    };
    return decodeValue<uint32_t>(value, decodeFunction);
}

bool ThingSetTextDecoder::decode(uint64_t *value)
{
    auto decodeFunction = [](const char *start, char **end) -> uint64_t {
        return static_cast<uint64_t>(std::strtoull(start, end, 10));
    };
    return decodeValue<uint64_t>(value, decodeFunction);
}

bool ThingSetTextDecoder::decode(int8_t *value)
{
    auto decodeFunction = [](const char *start, char **end) -> int8_t {
        return static_cast<int8_t>(std::strtol(start, end, 10));
    };
    return decodeValue<int8_t>(value, decodeFunction);
}

bool ThingSetTextDecoder::decode(int16_t *value)
{
    auto decodeFunction = [](const char *start, char **end) -> int16_t {
        return static_cast<int16_t>(std::strtol(start, end, 10));
    };
    return decodeValue<int16_t>(value, decodeFunction);
}

bool ThingSetTextDecoder::decode(int32_t *value)
{
    auto decodeFunction = [](const char *start, char **end) -> int32_t {
        return static_cast<int32_t>(std::strtol(start, end, 10));
    };
    return decodeValue<int32_t>(value, decodeFunction);
}

bool ThingSetTextDecoder::decode(int64_t *value)
{
    auto decodeFunction = [](const char *start, char **end) -> int64_t {
        return static_cast<int64_t>(std::strtol(start, end, 10));
    };
    return decodeValue<int64_t>(value, decodeFunction);
}

// todo check this
bool ThingSetTextDecoder::decodeNull()
{
    return true;
}

// todo check this
bool ThingSetTextDecoder::decodeListStart()
{
    if (_inputBuffer[_bufferElemPtr] == '[') {
        _bufferElemPtr++;
        return true;
    }
    return false;
}

// todo check this
bool ThingSetTextDecoder::decodeListEnd()
{
    if (_inputBuffer[_bufferElemPtr] == ']') {
        _bufferElemPtr++;
        return true;
    }
    return false;
}

bool ThingSetTextDecoder::decodeListSeparator()
{
    if (_inputBuffer[_bufferElemPtr] == ',') {
        _bufferElemPtr++;
        return true;
    }
    else if (_inputBuffer[_bufferElemPtr] == ']') { // end of list
        return true;
    }
    return false;
}

// todo implement this
bool ThingSetTextDecoder::decodeList(std::function<bool(size_t)> callback)
{
    // if (!decodeListStart()) {
    //     return false;
    // }

    // size_t index = 0;
    // while (getState()->elem_count != 0) {
    //     if (!callback(index++)) {
    //         return false;
    //     }
    // }

    // return decodeListEnd();
    (void)callback;
    return true;
}

} // namespace ThingSet