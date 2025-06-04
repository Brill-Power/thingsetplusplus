/*
 * Copyright (c) 2024-2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#ifdef __ZEPHYR__
#include <zephyr/logging/log.h>
namespace ThingSet::Logging {
LOG_MODULE_DECLARE(thingsetplusplus, CONFIG_THINGSET_PLUS_PLUS_LOG_LEVEL);
}

#define LOG_DEBUG(...) \
    { \
        using namespace ThingSet::Logging; \
        LOG_DBG(__VA_ARGS__); \
    }
#define LOG_INFO(...) \
    { \
        using namespace ThingSet::Logging; \
        LOG_INF(__VA_ARGS__); \
    }
#define LOG_WARN(...) \
    { \
        using namespace ThingSet::Logging; \
        LOG_WRN(__VA_ARGS__); \
    }
#define LOG_ERROR(...) \
    { \
        using namespace ThingSet::Logging; \
        LOG_ERR(__VA_ARGS__); \
    }

template <typename ... T>
void LOG_SMART(T... args)
{
    // no-op for now
}

#elif defined(DEBUG_LOGGING)
#include <iostream>

#define LOCAL_DEBUG_LEVEL 3

#define LOG_DEBUG(...)                                                                                                   \
    if (LOCAL_DEBUG_LEVEL > 0) {                                                                                       \
        printf(__VA_ARGS__);                                                                                           \
        printf("\n");                                                                                                  \
    }
#define LOG_INFO(...)                                                                                                   \
    if (LOCAL_DEBUG_LEVEL > 1) {                                                                                       \
        printf(__VA_ARGS__);                                                                                           \
        printf("\n");                                                                                                  \
    }
#define LOG_WARN(...)                                                                                                   \
    if (LOCAL_DEBUG_LEVEL > 0) {                                                                                       \
        printf(__VA_ARGS__);                                                                                           \
        printf("\n");                                                                                                  \
    }
#define LOG_ERROR(...)                                                                                                   \
    printf(__VA_ARGS__);                                                                                               \
    printf("\n");

template <typename ... T>
void LOG_SMART(T... args)
{
    (std::cout << ... << args) << std::endl;
}
#else

#ifndef LOG_DEBUG

#define LOG_DEBUG(...)
#define LOG_INFO(...)
#define LOG_WARN(...)
#define LOG_ERROR(...)

template <typename ... T>
void LOG_SMART(T...)
{
}

#endif

#endif
