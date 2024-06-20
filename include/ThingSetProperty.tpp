/*
 * Copyright (c) 2024 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */
#include "ThingSetProperty.hpp"

template <unsigned id, typename T>
ThingSet::ThingSetProperty<id, T>::ThingSetProperty(std::string_view name, T value)
    : ThingSet::IdentifiableThingSetNode<id>::IdentifiableThingSetNode(name), ThingSet::ThingSetValue<T>(value)
{}
