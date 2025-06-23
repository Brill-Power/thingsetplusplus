/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "thingset++/zephyr/ThingSetZephyrShellServerTransport.hpp"
#include "thingset++/ThingSetServer.hpp"
#include <zephyr/shell/shell.h>
#include <zephyr/shell/shell_uart.h>

namespace ThingSet::Zephyr {

ThingSetZephyrShellServerTransport::ThingSetZephyrShellServerTransport()
{}

StreamingZephyrShellThingSetBinaryEncoder ThingSetZephyrShellServerTransport::getPublishingEncoder()
{
    return StreamingZephyrShellThingSetBinaryEncoder();
}

bool ThingSetZephyrShellServerTransport::listen(std::function<int(const EmptyIdentifier &, uint8_t *, size_t, uint8_t *, size_t)> callback)
{
    _callback = callback;
    return true;
}

int ThingSetZephyrShellServerTransport::_onShellCommandExecuted(const shell *shell, size_t argc, char **argv)
{
    static ThingSetZephyrShellServerTransport transport;
    static auto server = ThingSetServerBuilder::build(transport);
    static bool initialised = false;
    if (!initialised)
    {
        server.listen();
        initialised = true;
    }
    return transport.onShellCommandExecuted(shell, argc, argv);
}

int ThingSetZephyrShellServerTransport::onShellCommandExecuted(const shell *shell, size_t argc, char **argv)
{
    size_t pos = 0;
    for (size_t cnt = 1; cnt < argc; cnt++) {
        int ret = snprintf(_requestBuffer.data() + pos, _requestBuffer.size() - pos, "%s ", argv[cnt]);
        if (ret < 0) {
            shell_print(shell, "Error: Request too large.");
            return ret;
        }
        pos += ret;
    }
    _requestBuffer.data()[--pos] = '\0';
    int result = _callback(EmptyIdentifier(), reinterpret_cast<uint8_t *>(_requestBuffer.data()), strlen(_requestBuffer.data()), _responseBuffer.data(), _responseBuffer.size());
    if (result > 0) {
        shell_print(shell, "%s", _responseBuffer.data());
    }
    return 0;
}

SHELL_CMD_ARG_REGISTER(thingset, NULL, "ThingSet request", ThingSetZephyrShellServerTransport::_onShellCommandExecuted, 1, 10);

} // namespace ThingSet::Zephyr
