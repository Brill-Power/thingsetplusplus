/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "thingset++/zephyr/StreamingZephyrShellThingSetBinaryEncoder.hpp"

namespace ThingSet::Zephyr {

StreamingZephyrShellThingSetBinaryEncoder::StreamingZephyrShellThingSetBinaryEncoder()
{}

bool StreamingZephyrShellThingSetBinaryEncoder::write(size_t length, bool)
{
    const shell *sh = shell_backend_uart_get_ptr();
    shell_print(sh, "%.*s", length, _buffer.data());
    return true;
}

} // namespace ThingSet::Zephyr