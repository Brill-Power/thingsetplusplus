/*
 * Copyright (c) 2024 Brill Power.
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
    bool ignored; // this field will be ignored by the encoder and decoder
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

TEST(BinaryRecords, EncodeAndDecodeSimpleRecord)
{
    SETUP()
    ASSERT_EQ(24.0f, moduleRecords[0].voltage.getValue());
    float &v1 = moduleRecords[0].voltage;
    v1 = 26.0f;
    ASSERT_EQ(26.0f, moduleRecords[0].voltage.getValue());
    float *vp1 = &moduleRecords[0].voltage;
    *vp1 = 27.0f;
    ASSERT_EQ(27.0f, moduleRecords[0].voltage.getValue());
    moduleRecords[0].voltage = 25.0f;

    uint8_t buffer[512];
    FixedDepthThingSetBinaryEncoder encoder(buffer, sizeof(buffer));
    encoder.encode(moduleRecords.getValue());
    ASSERT_EQ(0x82, buffer[0]); // array with 2 elements
    ASSERT_EQ(0xA5, buffer[1]); // map with 5 elements

    FixedDepthThingSetBinaryDecoder decoder(buffer, sizeof(buffer));
    std::array<ModuleRecord, 2> newModuleRecords;
    ASSERT_TRUE(decoder.decode(&newModuleRecords));
    ASSERT_EQ(moduleRecords[0].voltage.getValue(), newModuleRecords[0].voltage.getValue());
    ASSERT_EQ(moduleRecords[1].voltage.getValue(), newModuleRecords[1].voltage.getValue());
    ASSERT_EQ(moduleRecords[0].current.getValue(), newModuleRecords[0].current.getValue());
    ASSERT_EQ(moduleRecords[1].current.getValue(), newModuleRecords[1].current.getValue());
    ASSERT_EQ(moduleRecords[0].supercells.getValue()[0].soc.getValue(),
              newModuleRecords[0].supercells.getValue()[0].soc.getValue());
}

TEST(BinaryRecords, InitialiseRecordArrayCopy)
{
    SETUP()
    ThingSetReadOnlyProperty<0x800, 0x0, "Modules", std::array<ModuleRecord, 2>> records(moduleRecords.getValue());
    ASSERT_EQ(24.2f, records[1].voltage.getValue());
    uint8_t buffer[512];
    FixedDepthThingSetBinaryEncoder encoder(buffer, sizeof(buffer));
    ASSERT_TRUE(records.encode(encoder));
    ASSERT_EQ(0x82, buffer[0]); // array with 2 elements
    ASSERT_EQ(0xA5, buffer[1]); // map with 5 elements
}

TEST(BinaryRecords, RecordArrayIndexing)
{
    SETUP()
    ThingSetReadOnlyProperty<0x800, 0x0, "Modules", std::array<ModuleRecord, 2>> records(moduleRecords.getValue());
    ASSERT_EQ(24.2f, records[1].voltage.getValue());
    ModuleRecord *mod = &records[1];
    ASSERT_EQ(24.2f, mod->voltage.getValue());
}

TEST(BinaryRecords, InitialiseRecordArrayInline)
{
    SETUP()
    ThingSetReadOnlyProperty<0x800, 0x0, "Modules", std::array<ModuleRecord, 1>> records = { { (ModuleRecord){
        .voltage = 24.0f,
        .current = 10.0f,
        .error = 0,
    } } };

    ASSERT_EQ(24.0f, records[0].voltage.getValue());
    uint8_t buffer[512];
    FixedDepthThingSetBinaryEncoder encoder(buffer, sizeof(buffer));
    ASSERT_TRUE(records.encode(encoder));
    ASSERT_EQ(0x81, buffer[0]); // array with 2 elements
    ASSERT_EQ(0xA5, buffer[1]); // map with 5 elements
}