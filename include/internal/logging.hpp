/*
 * Copyright (c) 2024 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */
#ifdef DEBUG_LOGGING

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

#else

#define LOG_DBG(...)
#define LOG_INF(...)
#define LOG_WRN(...)
#define LOG_ERR(...)

#endif