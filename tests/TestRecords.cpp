#include "ThingSetProperty.hpp"
#include "ThingSetRegistry.hpp"
#include "gtest/gtest.h"

using namespace ThingSet;

struct ModuleRecord
{
    ThingSetProperty<0x601, "voltage", float> voltage;
    ThingSetProperty<0x602, "current", float> current;
    ThingSetProperty<0x603, "error", uint64_t> error;
    ThingSetProperty<0x604, "cellVoltages", std::array<float, 6>> cellVoltages;
};

template <typename TupleT, typename Fn> constexpr void for_each_tuple(TupleT &&tp, Fn &&fn)
{
    std::apply([&fn]<typename... T>(T &&...args) { (fn(std::forward<T>(args)), ...); }, std::forward<TupleT>(tp));
}

TEST(Records, SimpleRecord)
{
    std::array<ModuleRecord, 2> moduleRecords = { (ModuleRecord){
                                                      .voltage = 24.0f,
                                                      .current = 10.0f,
                                                      .error = 0,
                                                      .cellVoltages = { { 3.2f, 3.1f, 2.9f, 3.3f, 3.0f, 2.8f } },
                                                  },
                                                  (ModuleRecord){
                                                      .voltage = 24.2f,
                                                      .current = 5.0f,
                                                      .error = 0,
                                                      .cellVoltages = { { 3.1f, 3.3f, 3.0f, 3.1f, 3.2f, 2.95f } },
                                                  } };

    ASSERT_EQ(24.0f, moduleRecords[0].voltage.getValue());
    float &v1 = moduleRecords[0].voltage;
    v1 = 26.0f;
    ASSERT_EQ(26.0f, moduleRecords[0].voltage.getValue());
    float *vp1 = &moduleRecords[0].voltage;
    *vp1 = 27.0f;
    ASSERT_EQ(27.0f, moduleRecords[0].voltage.getValue());
    moduleRecords[0].voltage = 25.0f;

    uint8_t buffer[512];
    FixedSizeThingSetBinaryEncoder<512> encoder(buffer);
    encoder.encode(moduleRecords);
    ASSERT_EQ(0x82, buffer[0]); // array with 2 elements
    ASSERT_EQ(0xA4, buffer[1]); // map with 4 elements
}