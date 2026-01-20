/*
 * Copyright (c) 2026 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "thingset++/ThingSetResult.hpp"

namespace ThingSet {

ThingSetResult::ThingSetResult(ThingSetStatusCode code) : _code(code)
{}

ThingSetStatusCode ThingSetResult::code() const {
    return _code;
}

bool ThingSetResult::success() const {
    switch (_code) {
        case ThingSetStatusCode::content:
        case ThingSetStatusCode::changed:
        case ThingSetStatusCode::created:
        case ThingSetStatusCode::deleted:
            return true;
        default:
            return false;
    }
}

ThingSetResult::operator bool() const {
    return success();
}

}