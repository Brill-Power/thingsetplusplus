/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <cstdint>

namespace ThingSet {

/// @brief Specifies the type of a ThingSet request.
enum struct ThingSetBinaryRequestType : uint8_t
{
    /// @brief Retrieve all data from a path
    get = 0x01,
    /// @brief Execute a function
    exec = 0x02,
    /// @brief Delete data from an object
    remove = 0x04, // aka delete
    /// @brief Retrieve a part of the data from a path
    fetch = 0x05,
    /// @brief Append data to an object
    create = 0x06,
    /// @brief Update (overwrite) data of a path
    update = 0x07,
    /// @brief Forward a request to another node
    forward = 0x1c,
    desire = 0x1d,
    report = 0x1f
};

static inline bool operator==(const uint8_t &lhs, const ThingSetBinaryRequestType &rhs) {
    return lhs == (uint8_t)rhs;
}

static inline bool operator>=(const uint8_t &lhs, const ThingSetBinaryRequestType &rhs) {
    return lhs >= (uint8_t)rhs;
}

static inline bool operator<=(const uint8_t &lhs, const ThingSetBinaryRequestType &rhs) {
    return lhs <= (uint8_t)rhs;
}

static inline bool operator!=(const uint8_t &lhs, const ThingSetBinaryRequestType &rhs) {
    return !(lhs == rhs);
}

enum struct ThingSetTextRequestType : char
{
    /// @brief Retrieve all data from a path
    get = '?',
    /// @brief Execute a function
    exec = '!',
    /// @brief Delete data from an object
    remove = '-',
    /// @brief Retrieve a part of the data from a path
    fetch = get,
    /// @brief Append data to an object
    create = '+',
    /// @brief Update (overwrite) data of a path
    update = '=',
    /// @brief Forward a request to another node
    forward = '>',
    desire = '@',
    report = '#'
};

/// @brief Specifies the status of a ThingSet request.
enum ThingSetStatusCode : uint8_t
{
    /// @brief Answer to CREATE requests
    created = 0x81,
    /// @brief Answer to DELETE requests
    deleted = 0x82,
    /// @brief Answer to EXEC and UPDATE requests
    changed = 0x84,
    /// @brief Answer to GET and FETCH requests
    content = 0x85,
    badRequest = 0xa0,
    /// @brief Authentication needed
    unauthorised = 0xa1,
    /// @brief Trying to write read-only value
    forbidden = 0xa3,
    notFound = 0xa4,
    /// @brief If e.g. DELETE is not allowed for that object
    methodNotAllowed = 0xa5,
    requestIncomplete = 0xa8,
    /// @brief Configuration conflicts with other settings
    conflict = 0xa9,
    requestTooLarge = 0xad,
    /// @brief If e.g. trying to assign a string to an int
    unsupportedFormat = 0xaf,
    internalServerError = 0xc0,
    /// @brief Request method is not supported
    notImplemented = 0xc1,
    /// @brief Node cannot be reached through gateway
    gatewayTimeout = 0xc4,
    /// @brief Node is not a gateway
    notAGateway = 0xc5
};

} // namespace ThingSet