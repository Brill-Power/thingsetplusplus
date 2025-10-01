/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#ifdef __ZEPHYR__

namespace ThingSet::Ip::Sockets {

/**
 * @brief Wait for network to be ready (L4 connected)
 *
 * This function blocks until the network interface has L4 connectivity.
 * On Zephyr, this is handled automatically by a SYS_INIT hook that runs
 * before main(), so this function typically returns immediately.
 *
 * This works for both DHCP and static IP configurations.
 */
void wait_for_network_ready(void);

} // namespace ThingSet::Ip::Sockets

#endif // __ZEPHYR__
