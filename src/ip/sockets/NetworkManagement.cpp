/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "thingset++/internal/logging.hpp"

#include <zephyr/kernel.h>
#include <zephyr/net/net_if.h>
#include <zephyr/net/net_mgmt.h>
#include <zephyr/net/net_event.h>
#include <zephyr/init.h>

namespace ThingSet::Ip::Sockets {

static K_SEM_DEFINE(networkReady_sem, 0, 1);
static struct net_mgmt_event_callback networkMgmtCb;
static bool networkReady = false;

static void networkEventHandler(struct net_mgmt_event_callback *cb, uint32_t mgmt_event,
                                   struct net_if *iface)
{
    if (mgmt_event == NET_EVENT_IPV4_ADDR_ADD) {
        LOG_INFO("IPV4 address added");
        networkReady = true;
        k_sem_give(&networkReady_sem);
    }
}

static int networkCallbackInit(void)
{
    net_mgmt_init_event_callback(&networkMgmtCb, networkEventHandler,
                                 NET_EVENT_IPV4_ADDR_ADD);
    net_mgmt_add_event_callback(&networkMgmtCb);

    return 0;
}

SYS_INIT(networkCallbackInit, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);

void waitForNetworkReady(void)
{
    if (networkReady) {
        return;
    }

    net_if *iface = net_if_get_default();
    if (!iface) {
        LOG_WARN("No default network interface found");
        networkReady = true;
        return;
    }

    net_if_ipv4 *ipConfig;
    if (net_if_config_ipv4_get(iface, &ipConfig) == 0 && ipConfig) {
        for (int i = 0; i < NET_IF_MAX_IPV4_ADDR; i++) {
            if (ipConfig->unicast[i].ipv4.addr_state == NET_ADDR_PREFERRED) {
                LOG_INFO("Network already connected");
                networkReady = true;
                return;
            }
        }
    }

    LOG_INFO("Waiting for network connectivity...");
    if (k_sem_take(&networkReady_sem, K_SECONDS(CONFIG_THINGSET_PLUS_PLUS_NET_RDY_TIMEOUT)) == 0) {
        LOG_INFO("Network connectivity established");
    } else {
        LOG_WARN("Network connectivity timeout, proceeding anyway");
        networkReady = true;
    }
}

} // namespace ThingSet::Ip::Sockets
