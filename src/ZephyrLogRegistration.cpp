/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifdef __ZEPHYR__
#include <zephyr/logging/log.h>
namespace ThingSet::Logging {
LOG_MODULE_REGISTER(thingsetplusplus, CONFIG_THINGSET_PLUS_PLUS_LOG_LEVEL);
}
#endif