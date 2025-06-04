/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "thingset++/ThingSetFunction.hpp"
#include "thingset++/ThingSetGroup.hpp"
#include "thingset++/ThingSetProperty.hpp"

#ifdef __ZEPHYR__
#ifdef CONFIG_THINGSET_PLUS_PLUS_SERVER
// #include "thingset++/ThingSetServer.hpp"
#endif

#ifdef CONFIG_THINGSET_PLUS_PLUS_BACKEND_CAN
#ifdef CONFIG_THINGSET_PLUS_PLUS_SERVER
#include "thingset++/can/zephyr/ThingSetZephyrCanServerTransport.hpp"
#endif // CONFIG_THINGSET_PLUS_PLUS_SERVER
#ifdef CONFIG_THINGSET_PLUS_PLUS_CLIENT
#include "thingset++/can/zephyr/ThingSetZephyrCanClientTransport.hpp"
#include "thingset++/can/zephyr/ThingSetZephyrCanSubscriptionTransport.hpp"
#endif // CONFIG_THINGSET_PLUS_PLUS_CLIENT
#endif // CONFIG_THINGSET_PLUS_PLUS_BACKEND_CAN

#ifdef CONFIG_THINGSET_PLUS_PLUS_BACKEND_SOCKET
#ifdef CONFIG_THINGSET_PLUS_PLUS_SERVER
#include "thingset++/ip/sockets/ThingSetSocketServerTransport.hpp"
#endif // CONFIG_THINGSET_PLUS_PLUS_SERVER
#ifdef CONFIG_THINGSET_PLUS_PLUS_CLIENT
#include "thingset++/ip/sockets/ThingSetSocketClientTransport.hpp"
#include "thingset++/ip/sockets/ThingSetSocketSubscriptionTransport.hpp"
#endif // CONFIG_THINGSET_PLUS_PLUS_CLIENT
#endif // CONFIG_THINGSET_PLUS_PLUS_BACKEND_SOCKET

#endif // __ZEPHYR__