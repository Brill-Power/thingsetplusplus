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
    // todo make this work with _bufferElemPtr
    *value = _inputBuffer;
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
    // todo template this
    const char *startPtr = &_inputBuffer[_bufferElemPtr];
    char *endPtr;
    *value = std::strtod(startPtr, &endPtr);
    size_t consumed = endPtr - startPtr;
    _bufferElemPtr += consumed;
    return true;
}

bool ThingSetTextDecoder::decode(double *value)
{
    // todo template this
    const char *startPtr = &_inputBuffer[_bufferElemPtr];
    char *endPtr;
    *value = std::strtod(startPtr, &endPtr);
    size_t consumed = endPtr - startPtr;
    _bufferElemPtr += consumed;
    return true;
}

// todo simplify this?
bool ThingSetTextDecoder::decode(bool *value)
{
    uint8_t retVal = std::strtoul(_inputBuffer, NULL, 10);

    if (retVal > 1) {
        return false;
    }

    *value = retVal;

    return true;
}

bool ThingSetTextDecoder::decode(uint8_t *value)
{
    // todo template this
    const char *startPtr = &_inputBuffer[_bufferElemPtr];
    char *endPtr;
    *value = std::strtoul(startPtr, &endPtr, 10);
    size_t consumed = endPtr - startPtr;
    _bufferElemPtr += consumed;
    return true;
}

bool ThingSetTextDecoder::decode(uint16_t *value)
{
    // todo template this
    const char *startPtr = &_inputBuffer[_bufferElemPtr];
    char *endPtr;
    *value = std::strtoul(startPtr, &endPtr, 10);
    size_t consumed = endPtr - startPtr;
    _bufferElemPtr += consumed;
    return true;
}

bool ThingSetTextDecoder::decode(uint32_t *value)
{
    // todo template this
    const char *startPtr = &_inputBuffer[_bufferElemPtr];
    char *endPtr;
    *value = std::strtoul(startPtr, &endPtr, 10);
    size_t consumed = endPtr - startPtr;
    _bufferElemPtr += consumed;
    return true;
}

// todo check this
bool ThingSetTextDecoder::decode(uint64_t *value)
{
    // todo template this
    const char *startPtr = &_inputBuffer[_bufferElemPtr];
    char *endPtr;
    *value = std::strtoull(startPtr, &endPtr, 10);
    size_t consumed = endPtr - startPtr;
    _bufferElemPtr += consumed;
    return true;
}

bool ThingSetTextDecoder::decode(int8_t *value)
{
    // todo template this
    const char *startPtr = &_inputBuffer[_bufferElemPtr];
    char *endPtr;
    *value = std::strtol(startPtr, &endPtr, 10);
    size_t consumed = endPtr - startPtr;
    _bufferElemPtr += consumed;
    return true;
}

bool ThingSetTextDecoder::decode(int16_t *value)
{
    // todo template this
    const char *startPtr = &_inputBuffer[_bufferElemPtr];
    char *endPtr;
    *value = std::strtol(startPtr, &endPtr, 10);
    size_t consumed = endPtr - startPtr;
    _bufferElemPtr += consumed;
    return true;
}

bool ThingSetTextDecoder::decode(int32_t *value)
{
    // todo template this
    const char *startPtr = &_inputBuffer[_bufferElemPtr];
    char *endPtr;
    *value = std::strtol(startPtr, &endPtr, 10);
    size_t consumed = endPtr - startPtr;
    _bufferElemPtr += consumed;
    return true;
}

bool ThingSetTextDecoder::decode(int64_t *value)
{
    // todo template this
    const char *startPtr = &_inputBuffer[_bufferElemPtr];
    char *endPtr;
    *value = std::strtoll(startPtr, &endPtr, 10);
    size_t consumed = endPtr - startPtr;
    _bufferElemPtr += consumed;
    return true;
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