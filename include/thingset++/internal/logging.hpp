/*
 * Copyright (c) 2024-2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#ifdef __ZEPHYR__
#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(thingsetplusplus, CONFIG_THINGSET_PLUS_PLUS_LOG_LEVEL);
#elif defined(DEBUG_LOGGING)
#include <iostream>

#define LOCAL_DEBUG_LEVEL 3

#define LOG_DBG(...)                                                                                                   \
    if (LOCAL_DEBUG_LEVEL > 0) {                                                                                       \
        printf(__VA_ARGS__);                                                                                           \
        printf("\n");                                                                                                  \
    }
#define LOG_INF(...)                                                                                                   \
    if (LOCAL_DEBUG_LEVEL > 1) {                                                                                       \
        printf(__VA_ARGS__);                                                                                           \
        printf("\n");                                                                                                  \
    }
#define LOG_WRN(...)                                                                                                   \
    if (LOCAL_DEBUG_LEVEL > 0) {                                                                                       \
        printf(__VA_ARGS__);                                                                                           \
        printf("\n");                                                                                                  \
    }
#define LOG_ERR(...)                                                                                                   \
    printf(__VA_ARGS__);                                                                                               \
    printf("\n");

template <typename ... T>
void LOG_SMART(T... args)
{
    (std::cout << ... << args) << std::endl;
}
#else

#ifndef LOG_DBG

#define LOG_DBG(...)
#define LOG_INF(...)
#define LOG_WRN(...)
#define LOG_ERR(...)

template <typename ... T>
void LOG_SMART(T...)
{
}

#endif

#endif