/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "thingset++/ThingSet.hpp"
#include "gtest/gtest.h"

using namespace ThingSet;

struct SupercellRecord
{
    ThingSetReadOnlyProperty<0x611, 0x610, "soc", float> soc;
    ThingSetReadOnlyProperty<0x612, 0x610, "soh", float> soh;
};

struct ModuleRecord
{
    ThingSetReadOnlyProperty<0x601, 0x600, "voltage", float> voltage;
    ThingSetReadOnlyProperty<0x602, 0x600, "current", float> current;
    ThingSetReadOnlyProperty<0x603, 0x600, "error", uint64_t> error;
    ThingSetReadOnlyProperty<0x604, 0x600, "cellVoltages", std::array<float, 6>> cellVoltages;
    ThingSetReadOnlyProperty<0x609, 0x600, "supercells", std::array<SupercellRecord, 6>> supercells;
};

#define ASSERT_BUFFER_EQ(expected, actual, actual_len)                                                                 \
    ASSERT_EQ(strlen(expected), actual_len);                                                                           \
    for (size_t i = 0; i < strlen(expected); i++) {                                                                    \
        ASSERT_EQ(expected[i], actual[i]);                                                                             \
    }

#define SETUP()                                                                                                        \
    ThingSetReadOnlyProperty<0x610, 0, "Modules", std::array<ModuleRecord, 2>> moduleRecords = {                       \
        { (ModuleRecord){                                                                                              \
              .voltage = 24.0f,                                                                                        \
              .current = 10.0f,                                                                                        \
              .error = 0,                                                                                              \
              .cellVoltages = { { 3.2f, 3.1f, 2.9f, 3.3f, 0.0f, 2.8f } },                                              \
              .supercells = { { (SupercellRecord){                                                                     \
                                    .soc = 0.1,                                                                        \
                                    .soh = 1,                                                                          \
                                },                                                                                     \
                                (SupercellRecord){ .soc = 0.25, .soh = 1 },                                            \
                                (SupercellRecord){                                                                     \
                                    .soc = 0.5,                                                                        \
                                    .soh = 1,                                                                          \
                                },                                                                                     \
                                (SupercellRecord){                                                                     \
                                    .soc = 0.75,                                                                       \
                                    .soh = 1,                                                                          \
                                },                                                                                     \
                                (SupercellRecord){                                                                     \
                                    .soc = 0.8,                                                                        \
                                    .soh = 1,                                                                          \
                                },                                                                                     \
                                (SupercellRecord){                                                                     \
                                    .soc = 1,                                                                          \
                                    .soh = 1,                                                                          \
                                } } },                                                                                 \
          },                                                                                                           \
          (ModuleRecord){                                                                                              \
              .voltage = 24.2f,                                                                                        \
              .current = 5.0f,                                                                                         \
              .error = 0,                                                                                              \
              .cellVoltages = { { 3.1f, 3.3f, 3.0f, 3.1f, 3.2f, 2.95f } },                                             \
          } }                                                                                                          \
    };

TEST(TextRecords, InitialiseRecordArrayCopy)
{
    SETUP()
    ThingSetReadOnlyProperty<0x800, 0x0, "Modules", std::array<ModuleRecord, 2>> records(moduleRecords.getValue());
    ASSERT_EQ(24.2f, records[1].voltage.getValue());
    char buffer[TEXT_ENCODER_BUFFER_SIZE];
    size_t size = sizeof(buffer);
    ThingSetTextEncoder encoder(buffer, size);
    ASSERT_TRUE(records.encode(encoder));
    const char *expected =
        "[{\"voltage\":24.000000,\"current\":10.000000,\"error\":0,\"cellVoltages\":[3.200000,3.100000,2.900000,3."
        "300000,0.000000,2.800000],\"supercells\":[{\"soc\":0.100000,\"soh\":1.000000},{\"soc\":0.250000,\"soh\":1."
        "000000},{\"soc\":0.500000,\"soh\":1.000000},{\"soc\":0.750000,\"soh\":1.000000},{\"soc\":0.800000,\"soh\":1."
        "000000},{\"soc\":1.000000,\"soh\":1.000000}]},{\"voltage\":24.200001,\"current\":5.000000,\"error\":0,"
        "\"cellVoltages\":[3.100000,3.300000,3.000000,3.100000,3.200000,2.950000],\"supercells\":[{\"soc\":0.000000,"
        "\"soh\":0.000000},{\"soc\":0.000000,\"soh\":0.000000},{\"soc\":0.000000,\"soh\":0.000000},{\"soc\":0.000000,"
        "\"soh\":0.000000},{\"soc\":0.000000,\"soh\":0.000000},{\"soc\":0.000000,\"soh\":0.000000}]}]";
    ASSERT_BUFFER_EQ(expected, buffer, encoder.getEncodedLength());
}

TEST(TextRecords, InitialiseRecordArrayInline)
{
    SETUP()
    ThingSetReadOnlyProperty<0x800, 0x0, "Modules", std::array<ModuleRecord, 1>> records = { { (ModuleRecord){
        .voltage = 24.0f,
        .current = 10.0f,
        .error = 0,
    } } };

    ASSERT_EQ(24.0f, records[0].voltage.getValue());
    char buffer[TEXT_ENCODER_BUFFER_SIZE];
    size_t size = sizeof(buffer);
    ThingSetTextEncoder encoder(buffer, size);
    ASSERT_TRUE(records.encode(encoder));
    const char *expected =
        "[{\"voltage\":24.000000,\"current\":10.000000,\"error\":0,\"cellVoltages\":[0.000000,0.000000,0.000000,0."
        "000000,0.000000,0.000000],\"supercells\":[{\"soc\":0.000000,\"soh\":0.000000},{\"soc\":0.000000,\"soh\":0."
        "000000},{\"soc\":0.000000,\"soh\":0.000000},{\"soc\":0.000000,\"soh\":0.000000},{\"soc\":0.000000,\"soh\":0."
        "000000},{\"soc\":0.000000,\"soh\":0.000000}]}]";
    ASSERT_BUFFER_EQ(expected, buffer, encoder.getEncodedLength());
}