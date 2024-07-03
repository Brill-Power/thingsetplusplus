/*
 * Copyright (c) 2024 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */

#include "ThingSetClient.hpp"

ThingSet::ThingSetClient::ThingSetClient(ThingSetClientTransport &transport) : _transport(transport)
{}

void ThingSet::ThingSetClient::connect()
{
    _transport.connect();
}
