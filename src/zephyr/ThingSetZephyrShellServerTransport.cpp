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
    std::array<char, CONFIG_SHELL_CMD_BUFF_SIZE / 2> requestBuffer;
    std::array<uint8_t, CONFIG_SHELL_CMD_BUFF_SIZE * 2> responseBuffer;
    size_t pos = 0;
    for (size_t cnt = 1; cnt < argc; cnt++) {
        int ret = snprintf(requestBuffer.data() + pos, requestBuffer.size() - pos, "%s ", argv[cnt]);
        if (ret < 0) {
            shell_print(shell, "Error: Request too large.");
            return ret;
        }
        pos += ret;
    }
    requestBuffer.data()[--pos] = '\0';
    int result = _callback(EmptyIdentifier(), reinterpret_cast<uint8_t *>(requestBuffer.data()), pos, responseBuffer.data(), responseBuffer.size());
    if (result > 0) {
        responseBuffer[result] = '\0';
        shell_print(shell, "%s", responseBuffer.data());
    }
    return 0;
}

SHELL_CMD_ARG_REGISTER(thingset, NULL, "ThingSet request", ThingSetZephyrShellServerTransport::_onShellCommandExecuted, 1, 10);

} // namespace ThingSet::Zephyr
