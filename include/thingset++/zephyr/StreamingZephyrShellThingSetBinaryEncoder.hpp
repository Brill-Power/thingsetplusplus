/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "thingset++/StreamingThingSetBinaryEncoder.hpp"
#include <zephyr/shell/shell.h>
#include <zephyr/shell/shell_uart.h>

namespace ThingSet::Zephyr {

class ThingSetZephyrShellServerTransport;

class StreamingZephyrShellThingSetBinaryEncoder : public StreamingThingSetBinaryEncoder<CONFIG_SHELL_CMD_BUFF_SIZE>
{
public:
    StreamingZephyrShellThingSetBinaryEncoder(bool enhanced);

protected:
    bool write(size_t length, bool) override;
};

} // namespace ThingSet::Zephyr