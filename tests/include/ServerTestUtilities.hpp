/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <thingset++/ThingSetServer.hpp>
#include <cstdint>
#include <string>
#include <vector>

/// @brief A server with no transport, whose request handlers can be called directly.
class TestableServer : public ThingSet::_ThingSetServer
{
public:
    TestableServer() : _ThingSetServer(nullptr)
    {}

    bool listen() override
    {
        return true;
    }

    using _ThingSetServer::handleBinaryRequest;
    using _ThingSetServer::handleTextRequest;
};

/// @brief Parses hex bytes, which may be separated by spaces, e.g. "85 F6 FA3F99999A".
inline std::vector<uint8_t> fromHex(const std::string &hex)
{
    std::vector<uint8_t> bytes;
    std::string digits;
    for (char c : hex) {
        if (c != ' ') {
            digits += c;
        }
    }
    for (size_t i = 0; i + 1 < digits.size(); i += 2) {
        bytes.push_back((uint8_t)std::stoul(digits.substr(i, 2), nullptr, 16));
    }
    return bytes;
}
