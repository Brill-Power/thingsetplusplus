/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <cstdint>

#ifdef __ZEPHYR__
// Use Zephyr Kconfig values
#define THINGSET_CAN_MIN_ADDRESS CONFIG_THINGSET_CAN_NODE_ADDRESS_MIN
#define THINGSET_CAN_MAX_ADDRESS CONFIG_THINGSET_CAN_NODE_ADDRESS_MAX
#else
// Use CMake-defined values (set by target_compile_definitions in CMakeLists.txt)
// If not building with CMake, fall back to defaults
#ifndef THINGSET_CAN_MIN_ADDRESS
#define THINGSET_CAN_MIN_ADDRESS 0x01
#endif
#ifndef THINGSET_CAN_MAX_ADDRESS
#define THINGSET_CAN_MAX_ADDRESS 0xfd
#endif
#endif

namespace ThingSet::Can {

/// @brief Configuration constants for CAN addressing
struct CanConfig {
    /// @brief The minimum valid address for a node.
    static constexpr uint8_t minAddress = THINGSET_CAN_MIN_ADDRESS;
    /// @brief The maximum valid address for a node.
    static constexpr uint8_t maxAddress = THINGSET_CAN_MAX_ADDRESS;
};

} // namespace ThingSet::Can