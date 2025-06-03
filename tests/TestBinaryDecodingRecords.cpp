/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "thingset++/ThingSet.hpp"
// #include "thingset++/ThingSetBinaryEncoder.hpp"
// #include "thingset++/ThingSetBinaryDecoder.hpp"
#include "gtest/gtest.h"

using namespace ThingSet;

struct SimpleRecord
{
    ThingSetReadOnlyProperty<0x601, 0x600, "voltage", float> voltage;
    ThingSetReadOnlyProperty<0x602, 0x600, "current", float> current;
    ThingSetReadOnlyProperty<0x603, 0x600, "error", uint64_t> error;
};

struct SimplerRecord
{
    ThingSetReadOnlyProperty<0x601, 0x600, "voltage", float> voltage;
    ThingSetReadOnlyProperty<0x603, 0x600, "error", uint64_t> error;
};

TEST(BinaryRecords, DecodeToRecordWhichIsSubsetOfDataOnWire)
{
    ThingSetReadOnlyProperty<0x610, 0, "Simples", std::array<SimpleRecord, 1>> simples = {
        {
            (SimpleRecord){
                .voltage = 24.0f,
                .current = 10.0f,
                .error = 1,
            }
        }
    };

    ThingSetReadOnlyProperty<0x611, 0, "Simplers", std::array<SimplerRecord, 1>> simplers;

    std::array<uint8_t, 512> buffer;
    FixedDepthThingSetBinaryEncoder encoder(buffer);
    ASSERT_TRUE(simples.encode(encoder));

    FixedDepthThingSetBinaryDecoder decoder(buffer);
    ASSERT_TRUE(simples.decode(decoder));

    decoder = FixedDepthThingSetBinaryDecoder(buffer);
    ASSERT_TRUE(simplers.decode(decoder));
    ASSERT_EQ(24.0f, simplers[0].voltage.getValue());
    ASSERT_EQ(1, simplers[0].error.getValue());
}