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

static K_SEM_DEFINE(network_ready_sem, 0, 1);
static struct net_mgmt_event_callback network_mgmt_cb;
static bool network_ready = false;

static void network_event_handler(struct net_mgmt_event_callback *cb, uint32_t mgmt_event,
                                   struct net_if *iface)
{
    if (mgmt_event == NET_EVENT_IPV4_ADDR_ADD) {
        LOG_INFO("IPV4 address added");
        network_ready = true;
        k_sem_give(&network_ready_sem);
    }
}

static int network_callback_init(void)
{
    net_mgmt_init_event_callback(&network_mgmt_cb, network_event_handler,
                                 NET_EVENT_IPV4_ADDR_ADD);
    net_mgmt_add_event_callback(&network_mgmt_cb);

    return 0;
}

SYS_INIT(network_callback_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);

void wait_for_network_ready(void)
{
    if (network_ready) {
        return;
    }

    net_if *iface = net_if_get_default();
    if (!iface) {
        LOG_WARN("No default network interface found");
        network_ready = true;
        return;
    }

    net_if_ipv4 *ipConfig;
    if (net_if_config_ipv4_get(iface, &ipConfig) == 0 && ipConfig) {
        for (int i = 0; i < NET_IF_MAX_IPV4_ADDR; i++) {
            if (ipConfig->unicast[i].ipv4.addr_state == NET_ADDR_PREFERRED) {
                LOG_INFO("Network already connected");
                network_ready = true;
                return;
            }
        }
    }

    LOG_INFO("Waiting for network connectivity...");
    if (k_sem_take(&network_ready_sem, K_SECONDS(CONFIG_THINGSET_PLUS_PLUS_NET_RDY_TIMEOUT)) == 0) {
        LOG_INFO("Network connectivity established");
    } else {
        LOG_WARN("Network connectivity timeout, proceeding anyway");
        network_ready = true;
    }
}

} // namespace ThingSet::Ip::Sockets
