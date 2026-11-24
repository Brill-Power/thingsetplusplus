/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <zephyr/drivers/eeprom.h>
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

/* An EEPROM on which every write fails, to exercise the retry and give-up path. */
static int failingEepromWrites;

static int failingEepromRead(const struct device *, off_t, void *, size_t)
{
    return 0;
}

static int failingEepromWrite(const struct device *, off_t, const void *, size_t)
{
    failingEepromWrites++;
    return -EIO;
}

static size_t failingEepromSize(const struct device *)
{
    return 4096;
}

static const struct eeprom_driver_api failingEepromApi = {
    .read = failingEepromRead,
    .write = failingEepromWrite,
    .size = failingEepromSize,
};

DEVICE_DEFINE(failing_eeprom, "failing_eeprom", NULL, NULL, NULL, NULL, POST_KERNEL,
              CONFIG_KERNEL_INIT_PRIORITY_DEVICE, &failingEepromApi);

ZTEST(ZephyrPersistence, test_save_reports_write_failure)
{
    failingEepromWrites = 0;
    ThingSetPersistence persistence(DEVICE_GET(failing_eeprom));
    zassert_false(persistence.save());
    /* every attempt was made on the data, and no header was written over the top */
    zassert_equal(CONFIG_THINGSET_PLUS_PLUS_EEPROM_ACCESS_ATTEMPTS, failingEepromWrites);
}

static void *testSetup(void)
{
    return nullptr;
}

ZTEST_SUITE(ZephyrPersistence, NULL, testSetup, NULL, NULL, NULL);