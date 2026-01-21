/*
 * Copyright (c) 2026 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "thingset++/ThingSetStatus.hpp"

namespace ThingSet {

class ThingSetResult
{
private:
    const ThingSetStatusCode _code;

public:
    ThingSetResult(const ThingSetStatusCode code);

    bool success() const;
    ThingSetStatusCode code() const;

    explicit operator bool() const;
};

}