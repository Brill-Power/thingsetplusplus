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
    get = 0x01,
    exec = 0x02,
    remove = 0x04, // aka delete
    fetch = 0x05,
    create = 0x06,
    update = 0x07,
    desire = 0x1d,
    report = 0x1f
};

static inline bool operator==(const uint8_t &lhs, const ThingSetBinaryRequestType &rhs) {
    return lhs == (uint8_t)rhs;
}

static inline bool operator!=(const uint8_t &lhs, const ThingSetBinaryRequestType &rhs) {
    return !(lhs == rhs);
}

enum struct ThingSetTextRequestType : char
{
    get = '?',
    exec = '!',
    remove = '-',
    fetch = get,
    create = '+',
    update = '=',
    desire = '@',
    report = '#'
};

/// @brief Specifies the status of a ThingSet request.
enum ThingSetStatusCode : uint8_t
{
    created = 0x81,
    deleted = 0x82,
    changed = 0x84,
    content = 0x85,
    badRequest = 0xa0,
    unauthorised = 0xa1,
    forbidden = 0xa3,
    notFound = 0xa4,
    methodNotAllowed = 0xa5,
    requestIncomplete = 0xa8,
    conflict = 0xa9,
    requestTooLarge = 0xad,
    unsupportedFormat = 0xaf,
    internalServerError = 0xc0,
    notImplemented = 0xc1,
    gatewayTimeout = 0xc4,
    notAGateway = 0xc5
};

} // namespace ThingSet