/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "thingset++/ThingSetServerTransport.hpp"
#include "thingset++/zephyr/StreamingZephyrShellThingSetBinaryEncoder.hpp"
#include <array>

namespace ThingSet::Zephyr {

struct EmptyIdentifier {
};

class ThingSetZephyrShellServerTransport : public ThingSetServerTransport<EmptyIdentifier, CONFIG_SHELL_CMD_BUFF_SIZE, StreamingZephyrShellThingSetBinaryEncoder>
{
private:
    std::function<int(const EmptyIdentifier &, uint8_t *, size_t, uint8_t *, size_t)> _callback;

    ThingSetZephyrShellServerTransport();
    ThingSetZephyrShellServerTransport(const ThingSetZephyrShellServerTransport &) = delete;
    ThingSetZephyrShellServerTransport(ThingSetZephyrShellServerTransport &&) = delete;

public:
    StreamingZephyrShellThingSetBinaryEncoder getPublishingEncoder() override;
    bool listen(std::function<int(const EmptyIdentifier &, uint8_t *, size_t, uint8_t *, size_t)> callback) override;

    static int _onShellCommandExecuted(const shell *shell, size_t argc, char **argv);

private:
    int onShellCommandExecuted(const shell *shell, size_t argc, char **argv);
};

} // namespace ThingSet::Zephyr