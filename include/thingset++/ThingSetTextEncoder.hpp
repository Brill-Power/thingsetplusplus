/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "ThingSetEncoder.hpp"
#include <cstring>

#define TEXT_ENCODER_BUFFER_SIZE 1024

namespace ThingSet {

/// @brief Text protocol encoder for ThingSet.
class ThingSetTextEncoder : public ThingSetEncoder
{
private:
    char *_responseBuffer;
    size_t _responseSize;
    size_t _responsePosition;
    uint8_t _depth;

public:
    template <size_t Size>
    ThingSetTextEncoder(std::array<char, Size> buffer) : ThingSetTextEncoder(buffer.data(), buffer.size())
    {}
    ThingSetTextEncoder(char *buffer, size_t size)
        : _responseBuffer(buffer), _responseSize(size), _responsePosition(0), _depth(0)
    {}

    size_t getEncodedLength() const override
    {
        return _responsePosition;
    }

    using ThingSetEncoder::encode;
    bool encode(const std::string_view &value) override;
    bool encode(std::string_view &value) override;
    bool encode(const std::string &value) override;
    bool encode(std::string &value) override;
    bool encode(const char *value) override;
    bool encode(char *value) override;
    bool encode(const float &value) override;
    bool encode(float &value) override;
    bool encode(float *value) override;
    bool encode(const double &value) override;
    bool encode(double &value) override;
    bool encode(double *value) override;
    bool encode(const bool &value) override;
    bool encode(bool &value) override;
    bool encode(bool *value);
    bool encode(const uint8_t &value) override;
    bool encode(uint8_t &value) override;
    bool encode(uint8_t *value) override;
    bool encode(const uint16_t &value) override;
    bool encode(uint16_t &value) override;
    bool encode(uint16_t *value) override;
    bool encode(const uint32_t &value) override;
    bool encode(uint32_t &value) override;
    bool encode(uint32_t *value) override;
    bool encode(const uint64_t &value) override;
    bool encode(uint64_t &value) override;
    bool encode(uint64_t *value) override;
    bool encode(const int8_t &value) override;
    bool encode(int8_t &value) override;
    bool encode(int8_t *value);
    bool encode(const int16_t &value) override;
    bool encode(int16_t &value) override;
    bool encode(int16_t *value);
    bool encode(const int32_t &value) override;
    bool encode(int32_t &value) override;
    bool encode(int32_t *value) override;
    bool encode(const int64_t &value) override;
    bool encode(int64_t &value) override;
    bool encode(int64_t *value) override;
    bool encodeNull() override;
    bool encodePreamble() override;
    /// @brief Encode the start of a list. In forward-only encoding scenarios, you should
    /// use the overload which allows the number of elements in the list to be specified in
    /// advance.
    /// @return True if encoding succeeded, otherwise false.
    bool encodeListStart() override;
    /// @brief Encode the start of a list, specifying the number of elements the list
    /// will contain.
    /// @param count The number of elements in the list.
    /// @return True if encoding succeeded, otherwise false.
    bool encodeListStart(uint32_t count) override;
    bool encodeListEnd() override;
    /// @brief Encode the end of a list, specifying the number of elements the list
    /// contained. It should match the number passed to @ref encodeListStart.
    /// @param count The number of elements in the list.
    /// @return True if encoding succeeded, otherwise false.
    bool encodeListEnd(uint32_t count) override;
    bool encodeMapStart() override;
    /// @brief Encode the start of a map, specifying the number of key-value pairs the map
    /// will contain.
    /// @param count The number of pairs in the map.
    /// @return True if encoding succeeded, otherwise false.
    bool encodeMapStart(uint32_t count) override;
    bool encodeMapEnd() override;
    /// @brief Encode the end of a map, specifying the number of key-value pairs the map
    /// contained. It should match the number passed to @ref encodeMapStart.
    /// @param count The number of pairs in the map.
    /// @return True if encoding succeeded, otherwise false.
    bool encodeMapEnd(uint32_t count) override;

protected:
    bool encodeListSeparator() override;
    bool encodeKeyValuePairSeparator() override;

private:
    /// @brief Determine the length of the value as a string
    /// @return Number of characters in the string.
    template <typename T> inline int ensureBufferCapacity(T value, const char *format)
    {
        int bytesRequired = snprintf(nullptr, 0, format, value); // get size of value as a string

        if (_responseSize < _responsePosition + bytesRequired) {
            return false;
        }
        return true;
    }

    inline bool append(const char value)
    {
        if (_responsePosition >= _responseSize) {
            return false;
        }
        _responseBuffer[_responsePosition++] = value;
        return true;
    }

    /// @brief Add a value to the response buffer as a string
    /// @return True if successful, false otherwise
    template <typename T> bool appendFormat(T value, const char *format)
    {
        if (!ensureBufferCapacity(value, format)) {
            return false;
        }

        int bytesWritten =
            snprintf(_responseBuffer + getEncodedLength(), _responseSize - getEncodedLength(), format, value);
        _responsePosition += bytesWritten;

        return true;
    }
};

/// @brief Interface for values that can be encoded with a text encoder.
class ThingSetTextEncodable
{
public:
    virtual bool encode(ThingSetTextEncoder &encoder) = 0;
};

}; // namespace ThingSet
