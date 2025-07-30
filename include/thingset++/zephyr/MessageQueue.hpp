/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <array>
#include "zephyr/kernel.h"

namespace ThingSet::Zephyr {

/// @brief Encapsulates a Zephyr message queue.
/// @tparam T Type of items in the queue.
/// @tparam Size Depth of the queue.
template <typename T, size_t Size>
class MessageQueue
{
private:
    k_msgq _queue;
    std::array<char, Size * sizeof(T)> _buffer;

public:
    MessageQueue()
    {
        k_msgq_init(&_queue, _buffer.data(), sizeof(T), Size);
    }

    bool push(const T &value)
    {
        return k_msgq_put(&_queue, &value, K_NO_WAIT) == 0;
    }

    bool push(T &&value)
    {
        return k_msgq_put(&_queue, &value, K_NO_WAIT) == 0;
    }

    void push_back(const T &value)
    {
        push(value);
    }

    void push_back(T &&value)
    {
        push(value);
    }

    void pop()
    {
        pop_front();
    }

    T pop_front()
    {
        T result;
        k_msgq_get(&_queue, &result, K_FOREVER);
        return result;
    }
};

}; // namespace ThingSet::Zephyr