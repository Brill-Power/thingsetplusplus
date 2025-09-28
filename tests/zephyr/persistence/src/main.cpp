/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <zephyr/ztest.h>

#include <thingset++/ThingSetClient.hpp>
#include <thingset++/ThingSetServer.hpp>
#include <thingset++/ThingSetFunction.hpp>
#include <thingset++/ThingSetPersistence.hpp>
#include <array>

LOG_MODULE_REGISTER(thingsetplusplustest, CONFIG_THINGSET_PLUS_PLUS_LOG_LEVEL);

using namespace ThingSet;

static const struct device *eepromDevice = DEVICE_DT_GET(DT_CHOSEN(thingset_eeprom));

ThingSetReadWriteProperty<float> totalVoltage { 0x300, 0, "totalVoltage", 24.0f };

ThingSetReadWriteProperty<uint32_t, Subset::persisted> identifier { 0x20, 0, "identifier", 1 };

ZTEST(ZephyrPersistence, test_persistence)
{
    ThingSetPersistence persistence(eepromDevice);
    zassert_true(persistence.save());
    identifier = 10;
    zassert_true(persistence.load());
    zassert_equal(1, identifier.getValue());
}

static void *testSetup(void)
{
    return nullptr;
}

ZTEST_SUITE(ZephyrPersistence, NULL, testSetup, NULL, NULL, NULL);