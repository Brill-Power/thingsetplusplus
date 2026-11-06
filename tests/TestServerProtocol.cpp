/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// Request/response tests for the server, in binary and text mode. The cases,
// data model and expected responses follow the protocol and serde tests of
// thingset-node-c (tests/protocol and tests/serde), so they double as a check
// that the two implementations behave the same. IDs are those of
// thingset-node-c's test data plus 0xB000, to keep clear of other tests.
//
// Error responses are checked by status code only: thingset-node-c also sends
// a diagnostic message, which thingset++ does not.
//
// Tests prefixed DISABLED_ expect thingset-node-c's behaviour where thingset++
// currently differs; each says what thingset++ does instead. Run them with
// --gtest_also_run_disabled_tests.

#include "gtest/gtest.h"
#include <thingset++/ThingSet.hpp>
#include <thingset++/ThingSetFunction.hpp>
#include <thingset++/ThingSetGroup.hpp>
#include <thingset++/ThingSetServer.hpp>
#include <cmath>
#include <cstdio>
#include <limits>
#include <string>
#include <vector>

using namespace ThingSet;

namespace {

class TestableServer : public _ThingSetServer
{
public:
    TestableServer() : _ThingSetServer(nullptr)
    {}

    bool listen() override
    {
        return true;
    }

    using _ThingSetServer::handleBinaryRequest;
    using _ThingSetServer::handleTextRequest;
};

/// @brief Parses hex bytes, which may be separated by spaces, e.g. "85 F6 FA3F99999A".
std::vector<uint8_t> fromHex(const std::string &hex)
{
    std::vector<uint8_t> bytes;
    std::string digits;
    for (char c : hex) {
        if (c != ' ') {
            digits += c;
        }
    }
    for (size_t i = 0; i + 1 < digits.size(); i += 2) {
        bytes.push_back((uint8_t)std::stoul(digits.substr(i, 2), nullptr, 16));
    }
    return bytes;
}

std::string toHex(const uint8_t *bytes, size_t len)
{
    std::string hex;
    char byte[4];
    for (size_t i = 0; i < len; i++) {
        snprintf(byte, sizeof(byte), i == 0 ? "%02X" : " %02X", bytes[i]);
        hex += byte;
    }
    return hex;
}

/// @brief Prefix of a text response with the given status, e.g. ":A4".
std::string textStatus(ThingSetStatusCode status)
{
    char prefix[4];
    snprintf(prefix, sizeof(prefix), ":%02X", (uint8_t)status);
    return prefix;
}

/// @brief Encodes a string shorter than 24 bytes as CBOR.
std::vector<uint8_t> cborString(const std::string &s)
{
    std::vector<uint8_t> bytes { (uint8_t)(0x60 + s.size()) };
    bytes.insert(bytes.end(), s.begin(), s.end());
    return bytes;
}

} // namespace

/// A subset of thingset-node-c's test data (tests/common/data.c).
class ServerProtocol : public ::testing::Test
{
protected:
    TestableServer server;

    ThingSetReadWriteProperty<uint32_t> t_s { 0xB010, 0, "t_s", 1000 };

    ThingSetGroup<0xB200, 0, "Types"> types;
    ThingSetReadWriteProperty<bool> wBool { 0xB201, 0xB200, "wBool", true };
    ThingSetReadWriteProperty<uint8_t> wU8 { 0xB202, 0xB200, "wU8", 8 };
    ThingSetReadWriteProperty<int8_t> wI8 { 0xB203, 0xB200, "wI8", -8 };
    ThingSetReadWriteProperty<uint16_t> wU16 { 0xB204, 0xB200, "wU16", 16 };
    ThingSetReadWriteProperty<int16_t> wI16 { 0xB205, 0xB200, "wI16", -16 };
    ThingSetReadWriteProperty<uint32_t> wU32 { 0xB206, 0xB200, "wU32", 32 };
    ThingSetReadWriteProperty<int32_t> wI32 { 0xB207, 0xB200, "wI32", -32 };
    ThingSetReadWriteProperty<uint64_t> wU64 { 0xB208, 0xB200, "wU64", 64 };
    ThingSetReadWriteProperty<int64_t> wI64 { 0xB209, 0xB200, "wI64", -64 };
    ThingSetReadWriteProperty<float> wF32 { 0xB20A, 0xB200, "wF32", -3.2f };
    ThingSetReadWriteProperty<std::string> wString { 0xB20C, 0xB200, "wString", std::string("string") };

    ThingSetGroup<0xB300, 0, "Arrays"> arrays;
    ThingSetReadWriteProperty<std::array<int32_t, 3>> wI32Array { 0xB307, 0xB300, "wI32", { -1, -2, -3 } };
    ThingSetReadWriteProperty<std::array<float, 3>> wF32Array { 0xB30A, 0xB300, "wF32", { -1.1f, -2.2f, -3.3f } };

    bool xVoidCalled = false;
    bool lBool = false;
    std::string uString;
    int32_t nNumber = 0;
    ThingSetGroup<0xB400, 0, "Exec"> exec;
    ThingSetUserFunction<0xB401, 0xB400, "xVoid", void> xVoid { [this]() { xVoidCalled = true; } };
    ThingSetNamedUserFunction<0xB402, 0xB400, "xVoidParams", ThingSetParameterNames<"lBool">, void, bool>
        xVoidParams { [this](bool b) { lBool = b; } };
    ThingSetNamedUserFunction<0xB404, 0xB400, "xI32Params", ThingSetParameterNames<"uString", "nNumber">, int32_t,
                              std::string, int32_t>
        xI32Params { [this](std::string s, int32_t n) {
            uString = s;
            nNumber = n;
            return -1;
        } };

    ThingSetGroup<0xB500, 0, "Access"> access;
    ThingSetReadOnlyProperty<float> rItem { 0xB501, 0xB500, "rItem", 3.14f };
    ThingSetReadWriteProperty<float> wItem { 0xB502, 0xB500, "wItem", 3.14f };

    ThingSetGroup<0xB700, 0, "Nested"> nested;
    ThingSetReadWriteProperty<int32_t> rBeginning { 0xB701, 0xB700, "rBeginning", 1 };
    ThingSetGroup<0xB702, 0xB700, "Obj1"> obj1;
    ThingSetReadWriteProperty<float> obj1Item1 { 0xB703, 0xB702, "rItem1_V", 1.1f };
    ThingSetReadWriteProperty<float> obj1Item2 { 0xB704, 0xB702, "rItem2_V", 1.2f };
    ThingSetReadWriteProperty<int32_t> rBetween { 0xB705, 0xB700, "rBetween", 2 };
    ThingSetGroup<0xB706, 0xB700, "Obj2"> obj2;
    ThingSetReadWriteProperty<float> obj2Item1 { 0xB707, 0xB706, "rItem1_V", 2.1f };
    ThingSetReadWriteProperty<float> obj2Item2 { 0xB708, 0xB706, "rItem2_V", 2.2f };
    ThingSetReadWriteProperty<int32_t> rEnd { 0xB709, 0xB700, "rEnd", 3 };

    ::testing::AssertionResult binary(const std::vector<uint8_t> &request, const std::vector<uint8_t> &expected)
    {
        std::vector<uint8_t> req = request;
        uint8_t response[512];
        int len = server.handleBinaryRequest(req.data(), req.size(), response, sizeof(response));
        if (len == (int)expected.size() && memcmp(response, expected.data(), len) == 0) {
            return ::testing::AssertionSuccess();
        }
        return ::testing::AssertionFailure() << "request:  " << toHex(request.data(), request.size())
                                             << "\nexpected: " << toHex(expected.data(), expected.size())
                                             << "\nactual:   " << (len > 0 ? toHex(response, len) : std::to_string(len));
    }

    /// @brief Sends a binary request and compares the whole response.
    ::testing::AssertionResult binary(const char *requestHex, const char *expectedHex)
    {
        return binary(fromHex(requestHex), fromHex(expectedHex));
    }

    /// @brief Sends a binary request and checks only the status code of the response.
    ::testing::AssertionResult binary(const char *requestHex, ThingSetStatusCode expected)
    {
        std::vector<uint8_t> req = fromHex(requestHex);
        uint8_t response[512];
        int len = server.handleBinaryRequest(req.data(), req.size(), response, sizeof(response));
        if (len > 0 && response[0] == (uint8_t)expected) {
            return ::testing::AssertionSuccess();
        }
        return ::testing::AssertionFailure() << "request:  " << requestHex << "\nexpected status: "
                                             << toHex((const uint8_t *)&expected, 1)
                                             << "\nactual:   " << (len > 0 ? toHex(response, len) : std::to_string(len));
    }

    std::string sendText(const std::string &request)
    {
        std::string req = request;
        uint8_t response[512];
        int len = server.handleTextRequest((uint8_t *)req.data(), req.size(), response, sizeof(response));
        return len > 0 ? std::string((char *)response, len) : "<" + std::to_string(len) + ">";
    }

    /// @brief Sends a text request and compares the whole response.
    ::testing::AssertionResult text(const std::string &request, const std::string &expected)
    {
        std::string actual = sendText(request);
        if (actual == expected) {
            return ::testing::AssertionSuccess();
        }
        return ::testing::AssertionFailure() << "request:  " << request << "\nexpected: '" << expected
                                             << "'\nactual:   '" << actual << "'";
    }

    /// @brief Sends a text request and checks only the status code of the response.
    ::testing::AssertionResult text(const std::string &request, ThingSetStatusCode expected)
    {
        std::string actual = sendText(request);
        if (actual.compare(0, 3, textStatus(expected)) == 0) {
            return ::testing::AssertionSuccess();
        }
        return ::testing::AssertionFailure() << "request:  " << request << "\nexpected status: " << textStatus(expected)
                                             << "\nactual:   '" << actual << "'";
    }
};

// ---- binary: get

TEST_F(ServerProtocol, BinaryGetSingleValueById)
{
    ASSERT_TRUE(binary("01 19 B704", "85 F6 FA 3F99999A"));
}

TEST_F(ServerProtocol, BinaryGetSingleValueByName)
{
    ASSERT_TRUE(binary("01 74 4E65737465642F4F626A312F724974656D325F56", /* Nested/Obj1/rItem2_V */
                       "85 F6 FA 3F99999A"));
}

/// Disabled: thingset++ currently encodes subgroups in full rather than as null.
TEST_F(ServerProtocol, DISABLED_BinaryGetNestedIds)
{
    ASSERT_TRUE(binary("01 19 B700", "85 F6 A5 "
                                     "19 B701 01 " /* rBeginning */
                                     "19 B702 F6 " /* Obj1 */
                                     "19 B705 02 " /* rBetween */
                                     "19 B706 F6 " /* Obj2 */
                                     "19 B709 03"  /* rEnd */));
}

/// Disabled: thingset++ currently encodes subgroups in full, and uses IDs as keys although the request used a name.
TEST_F(ServerProtocol, DISABLED_BinaryGetNestedNames)
{
    ASSERT_TRUE(binary("01 66 4E6573746564", /* Nested */
                       "85 F6 A5 "
                       "6A 72426567696E6E696E67 01 " /* rBeginning */
                       "64 4F626A31 F6 "             /* Obj1 */
                       "68 724265747765656E 02 "     /* rBetween */
                       "64 4F626A32 F6 "             /* Obj2 */
                       "64 72456E64 03"              /* rEnd */));
}

/// Disabled: thingset++ currently leaves functions out of a group, giving an empty map.
TEST_F(ServerProtocol, DISABLED_BinaryGetExecIds)
{
    ASSERT_TRUE(binary("01 19 B400", "85 F6 A3 "
                                     "19 B401 80 "                                    /* xVoid */
                                     "19 B402 81 65 6C426F6F6C "                      /* xVoidParams */
                                     "19 B404 82 67 75537472696E67 67 6E4E756D626572" /* xI32Params */));
}

TEST_F(ServerProtocol, BinaryGetInt32Array)
{
    ASSERT_TRUE(binary("01 19 B307", "85 F6 83 20 21 22")); /* [-1,-2,-3] */
}

TEST_F(ServerProtocol, BinaryGetFloatArray)
{
    ASSERT_TRUE(binary("01 19 B30A", "85 F6 83 FA BF8CCCCD FA C00CCCCD FA C0533333")); /* [-1.1,-2.2,-3.3] */
}

TEST_F(ServerProtocol, BinaryGetInfinity)
{
    wF32 = std::numeric_limits<float>::infinity();
    ASSERT_TRUE(binary("01 19 B20A", "85 F6 FA 7F800000"));
}

TEST_F(ServerProtocol, BinaryGetNotFound)
{
    ASSERT_TRUE(binary("01 19 B7FF", ThingSetStatusCode::notFound));
    ASSERT_TRUE(binary("01 63 666F6F", ThingSetStatusCode::notFound)); /* foo */
}

// ---- binary: fetch

TEST_F(ServerProtocol, BinaryFetchNestedIds)
{
    ASSERT_TRUE(binary("05 19 B700 F6", "85 F6 85 19 B701 19 B702 19 B705 19 B706 19 B709"));
}

TEST_F(ServerProtocol, BinaryFetchNestedNames)
{
    ASSERT_TRUE(binary("05 66 4E6573746564 F6", "85 F6 85 "
                                                "6A 72426567696E6E696E67 " /* rBeginning */
                                                "64 4F626A31 "             /* Obj1 */
                                                "68 724265747765656E "     /* rBetween */
                                                "64 4F626A32 "             /* Obj2 */
                                                "64 72456E64"              /* rEnd */));
}

TEST_F(ServerProtocol, BinaryFetchBadElement)
{
    ASSERT_TRUE(binary("05 00 81 F5", ThingSetStatusCode::badRequest)); /* ? [true] */
}

/// Disabled: thingset++ currently only supports fetching child names or IDs (null) and metadata: badRequest.
TEST_F(ServerProtocol, DISABLED_BinaryFetchMultipleByName)
{
    ASSERT_TRUE(binary("05 65 5479706573 83 "    /* ?Types [ */
                       "64 77463332 "            /* "wF32", */
                       "65 77426F6F6C "          /* "wBool", */
                       "64 77553332",            /* "wU32"] */
                       "85 F6 83 FA C04CCCCD F5 18 20")); /* [-3.2,true,32] */
}

/// Disabled: thingset++ currently only supports fetching child names or IDs (null) and metadata: badRequest.
TEST_F(ServerProtocol, DISABLED_BinaryFetchById)
{
    ASSERT_TRUE(binary("05 19 B200 81 19 B20A", "85 F6 81 FA C04CCCCD")); /* ?Types [wF32] */
}

/// Disabled: thingset++ currently only supports fetching child names or IDs (null) and metadata: badRequest.
TEST_F(ServerProtocol, DISABLED_BinaryFetchNotFound)
{
    ASSERT_TRUE(binary("05 65 5479706573 81 63 666F6F", ThingSetStatusCode::notFound)); /* ?Types ["foo"] */
}

// ---- binary: update

TEST_F(ServerProtocol, BinaryUpdateTimestampZero)
{
    ASSERT_TRUE(binary("07 00 A1 19 B010 00", "84 F6 F6"));
    ASSERT_EQ(0u, t_s.getValue());
}

TEST_F(ServerProtocol, BinaryUpdateByName)
{
    ASSERT_TRUE(binary("07 65 5479706573 A1 64 77493332 18 34", "84 F6 F6")); /* =Types {"wI32":52} */
    ASSERT_EQ(52, wI32.getValue());
}

TEST_F(ServerProtocol, BinaryUpdate16BitFloat)
{
    ASSERT_TRUE(binary("07 19 B200 A1 19 B20A F9 3C00", "84 F6 F6"));
    ASSERT_EQ(1.0f, wF32.getValue());
}

TEST_F(ServerProtocol, BinaryUpdateInt32Array)
{
    ASSERT_TRUE(binary("07 19 B300 A1 19 B307 83 01 02 03", "84 F6 F6")); /* =Arrays {"wI32":[1,2,3]} */
    std::array<int32_t, 3> expected { 1, 2, 3 };
    ASSERT_EQ(expected, wI32Array.getValue());
}

TEST_F(ServerProtocol, BinaryUpdateReadOnly)
{
    ASSERT_TRUE(binary("07 19 B500 A1 19 B501 18 34", ThingSetStatusCode::forbidden)); /* =Access {"rItem":52} */
    ASSERT_EQ(3.14f, rItem.getValue());
}

TEST_F(ServerProtocol, BinaryUpdateWrongPath)
{
    ASSERT_TRUE(binary("07 64 54797065 A1 64 77493332 18 34", ThingSetStatusCode::notFound)); /* =Type {"wI32":52} */
}

TEST_F(ServerProtocol, BinaryUpdateUnknownObject)
{
    ASSERT_TRUE(binary("07 65 5479706573 A1 63 774933 18 34", ThingSetStatusCode::notFound)); /* =Types {"wI3":52} */
}

/// thingset-node-c checks every item before writing any of them, so a request
/// that fails changes nothing.
/// Disabled: thingset++ currently writes the items before the one that fails.
TEST_F(ServerProtocol, DISABLED_BinaryUpdateIsAllOrNothing)
{
    /* =Types {"wI32":1,"wI3":52} */
    ASSERT_TRUE(binary("07 65 5479706573 A2 64 77493332 01 63 774933 18 34", ThingSetStatusCode::notFound));
    ASSERT_EQ(-32, wI32.getValue());
}

/// Values that do not fit the target type are rejected rather than wrapped.
TEST_F(ServerProtocol, BinaryUpdateOutOfRange)
{
    EXPECT_TRUE(binary("07 65 5479706573 A1 63 775538 19 0100", ThingSetStatusCode::badRequest)); /* wU8: 256 */
    EXPECT_TRUE(binary("07 65 5479706573 A1 63 774938 18 80", ThingSetStatusCode::badRequest));   /* wI8: 128 */
    EXPECT_TRUE(binary("07 65 5479706573 A1 63 774938 38 80", ThingSetStatusCode::badRequest));   /* wI8: -129 */
    EXPECT_EQ(8, wU8.getValue());
    EXPECT_EQ(-8, wI8.getValue());
}

// ---- binary: exec

TEST_F(ServerProtocol, BinaryExecVoidById)
{
    ASSERT_TRUE(binary("02 19 B401 80", "84 F6 F6"));
    ASSERT_TRUE(xVoidCalled);
}

TEST_F(ServerProtocol, BinaryExecVoidByName)
{
    ASSERT_TRUE(binary("02 6A 457865632F78566F6964 80", "84 F6 F6")); /* !Exec/xVoid */
    ASSERT_TRUE(xVoidCalled);
}

TEST_F(ServerProtocol, BinaryExecVoidWithParams)
{
    ASSERT_TRUE(binary("02 19 B402 81 F5", "84 F6 F6")); /* [true] */
    ASSERT_TRUE(lBool);
}

TEST_F(ServerProtocol, BinaryExecInt32)
{
    ASSERT_TRUE(binary("02 19 B404 82 64 74657374 18 7B", "84 F6 20")); /* ["test",123] -> -1 */
    ASSERT_EQ("test", uString);
    ASSERT_EQ(123, nNumber);
}

TEST_F(ServerProtocol, BinaryExecInvalidParams)
{
    ASSERT_TRUE(binary("02 19 B402 F5", ThingSetStatusCode::badRequest));          /* true, not [true] */
    ASSERT_TRUE(binary("02 19 B402 82 F5 18 7B", ThingSetStatusCode::badRequest)); /* too many */
    ASSERT_TRUE(binary("02 19 B402 80", ThingSetStatusCode::badRequest));          /* not enough */
}

/// Disabled: thingset++ currently responds badRequest.
TEST_F(ServerProtocol, DISABLED_BinaryExecWrongParamType)
{
    ASSERT_TRUE(binary("02 19 B402 81 65 77726F6E67", ThingSetStatusCode::unsupportedFormat)); /* ["wrong"] */
}

/// Disabled: thingset++ currently responds badRequest.
TEST_F(ServerProtocol, DISABLED_BinaryExecNotExecutable)
{
    ASSERT_TRUE(binary("02 6C 4163636573732F724974656D 80", ThingSetStatusCode::forbidden)); /* !Access/rItem */
}

// ---- text: get

TEST_F(ServerProtocol, TextGetSingleValue)
{
    ASSERT_TRUE(text("?Nested/rBeginning", ":85 1"));
}

/// Disabled: thingset++ currently always writes six decimal places: 1.200000.
TEST_F(ServerProtocol, DISABLED_TextGetSingleFloat)
{
    ASSERT_TRUE(text("?Nested/Obj1/rItem2_V", ":85 1.2"));
}

/// Disabled: thingset++ currently encodes subgroups in full rather than as null.
TEST_F(ServerProtocol, DISABLED_TextGetNested)
{
    ASSERT_TRUE(text("?Nested", ":85 {\"rBeginning\":1,\"Obj1\":null,\"rBetween\":2,\"Obj2\":null,\"rEnd\":3}"));
}

/// Disabled: thingset++ currently leaves functions out of a group, giving {}.
TEST_F(ServerProtocol, DISABLED_TextGetExec)
{
    ASSERT_TRUE(text("?Exec", ":85 {\"xVoid\":[],\"xVoidParams\":[\"lBool\"],\"xI32Params\":[\"uString\",\"nNumber\"]}"));
}

TEST_F(ServerProtocol, TextGetNotFound)
{
    ASSERT_TRUE(text("?Foo", ThingSetStatusCode::notFound));
}

/// Disabled: thingset++ currently writes inf and nan, which are not valid JSON.
TEST_F(ServerProtocol, DISABLED_TextGetNonFiniteFloat)
{
    wF32 = std::numeric_limits<float>::infinity();
    ASSERT_TRUE(text("?Types/wF32", ":85 null"));
    wF32 = std::nanf("");
    ASSERT_TRUE(text("?Types/wF32", ":85 null"));
}

/// Disabled: thingset++ currently does not escape strings, so the output is not valid JSON.
TEST_F(ServerProtocol, DISABLED_TextGetEscapedString)
{
    wString = std::string("\n\"\\");
    ASSERT_TRUE(text("?Types/wString", ":85 \"\\n\\\"\\\\\""));
}

// ---- text: fetch

/// Disabled: thingset++ currently treats this as a get and returns the whole group.
TEST_F(ServerProtocol, DISABLED_TextFetchNestedNames)
{
    ASSERT_TRUE(text("?Nested null", ":85 [\"rBeginning\",\"Obj1\",\"rBetween\",\"Obj2\",\"rEnd\"]"));
}

/// Disabled: thingset++ currently treats this as a get of the root.
TEST_F(ServerProtocol, DISABLED_TextFetchBadElement)
{
    ASSERT_TRUE(text("? [true]", ThingSetStatusCode::badRequest));
}

/// Disabled: thingset++ currently treats this as a get and returns the whole group.
TEST_F(ServerProtocol, DISABLED_TextFetchMultiple)
{
    ASSERT_TRUE(text("?Types [\"wF32\",\"wBool\",\"wU32\"]", ":85 [-3.2,true,32]"));
}

/// Disabled: thingset++ currently treats this as a get and returns the whole group.
TEST_F(ServerProtocol, DISABLED_TextFetchNotFound)
{
    ASSERT_TRUE(text("?Types [\"foo\"]", ThingSetStatusCode::notFound));
}

// ---- text: update

/// Disabled: thingset++ currently stores escape sequences as written, e.g. a backslash and n.
TEST_F(ServerProtocol, DISABLED_TextUpdateEscapedString)
{
    ASSERT_TRUE(text("=Types {\"wString\":\"\\n\\\"\\\\\"}", ThingSetStatusCode::changed));
    ASSERT_EQ("\n\"\\", wString.getValue());
}

TEST_F(ServerProtocol, TextUpdateWhitespace)
{
    ASSERT_TRUE(text("=Types {    \"wF32\" : 52.8,\"wI32\":50.6}", ThingSetStatusCode::changed));
    ASSERT_EQ(52.8f, wF32.getValue());
    ASSERT_EQ(50, wI32.getValue());
}

TEST_F(ServerProtocol, TextUpdateOutOfRange)
{
    EXPECT_TRUE(text("=Types {\"wU8\":256}", ThingSetStatusCode::badRequest));
    EXPECT_TRUE(text("=Types {\"wI8\":-129}", ThingSetStatusCode::badRequest));
    EXPECT_EQ(8, wU8.getValue());
    EXPECT_EQ(-8, wI8.getValue());
}

TEST_F(ServerProtocol, TextUpdateInt32Array)
{
    ASSERT_TRUE(text("=Arrays {\"wI32\":[1,2,3]}", ThingSetStatusCode::changed));
    std::array<int32_t, 3> expected { 1, 2, 3 };
    ASSERT_EQ(expected, wI32Array.getValue());
}

TEST_F(ServerProtocol, TextUpdateReadOnly)
{
    ASSERT_TRUE(text("=Access {\"rItem\" : 52}", ThingSetStatusCode::forbidden));
    ASSERT_EQ(3.14f, rItem.getValue());
}

TEST_F(ServerProtocol, TextUpdateUnknownObject)
{
    ASSERT_TRUE(text("=Types {\"wI3\" : 52}", ThingSetStatusCode::notFound));
}

TEST_F(ServerProtocol, TextUpdateInvalidEndpoint)
{
    ASSERT_TRUE(text("=Types{\"wF32\":54.3}", ThingSetStatusCode::notFound));
}

/// Disabled: thingset++ currently accepts this and responds changed.
TEST_F(ServerProtocol, DISABLED_TextUpdateMalformedJson)
{
    ASSERT_TRUE(text("=Types [\"wF32\":54.3", ThingSetStatusCode::badRequest));
    ASSERT_EQ(-3.2f, wF32.getValue());
}

// ---- text: exec

/// Disabled: thingset++ currently requires an empty list: !Exec/xVoid [].
TEST_F(ServerProtocol, DISABLED_TextExecVoid)
{
    ASSERT_TRUE(text("!Exec/xVoid", ThingSetStatusCode::changed));
    ASSERT_TRUE(xVoidCalled);
}

TEST_F(ServerProtocol, TextExecVoidWithParams)
{
    ASSERT_TRUE(text("!Exec/xVoidParams [true]", ThingSetStatusCode::changed));
    ASSERT_TRUE(lBool);
}

TEST_F(ServerProtocol, TextExecInt32)
{
    ASSERT_TRUE(text("!Exec/xI32Params [\"test\",123]", ":84 -1"));
    ASSERT_EQ("test", uString);
    ASSERT_EQ(123, nNumber);
}

TEST_F(ServerProtocol, TextExecInvalidParams)
{
    ASSERT_TRUE(text("!Exec/xVoidParams [true", ThingSetStatusCode::badRequest));       /* invalid JSON */
    ASSERT_TRUE(text("!Exec/xVoidParams true", ThingSetStatusCode::badRequest));        /* not an array */
    ASSERT_TRUE(text("!Exec/xVoidParams [true, 123]", ThingSetStatusCode::badRequest)); /* too many */
}

/// Disabled: thingset++ currently responds badRequest.
TEST_F(ServerProtocol, DISABLED_TextExecWrongParamType)
{
    ASSERT_TRUE(text("!Exec/xVoidParams [\"wrong\"]", ThingSetStatusCode::unsupportedFormat));
}

/// Disabled: thingset++ currently responds badRequest.
TEST_F(ServerProtocol, DISABLED_TextExecNotExecutable)
{
    ASSERT_TRUE(text("!Access/rItem", ThingSetStatusCode::forbidden));
}

// ---- serde: a value written in one mode reads back correctly in the other

struct SerdeCase
{
    const char *name;
    const char *json;
    const char *cbor;
};

class ServerSerde : public ServerProtocol
{
protected:
    /// @brief Updates Types/<name> in text mode, then checks a binary get returns the expected CBOR.
    void textUpdateBinaryGet(const std::vector<SerdeCase> &cases)
    {
        for (const auto &c : cases) {
            SCOPED_TRACE(std::string(c.name) + " = " + c.json);
            EXPECT_TRUE(text(std::string("=Types {\"") + c.name + "\":" + c.json + "}", ThingSetStatusCode::changed));

            std::vector<uint8_t> request { (uint8_t)ThingSetBinaryRequestType::get };
            auto path = cborString(std::string("Types/") + c.name);
            request.insert(request.end(), path.begin(), path.end());
            std::vector<uint8_t> expected { (uint8_t)ThingSetStatusCode::content, 0xF6 };
            auto value = fromHex(c.cbor);
            expected.insert(expected.end(), value.begin(), value.end());
            EXPECT_TRUE(binary(request, expected));
        }
    }

    /// @brief Updates Types/<name> in binary mode, then checks a text get returns the expected JSON.
    void binaryUpdateTextGet(const std::vector<SerdeCase> &cases)
    {
        for (const auto &c : cases) {
            SCOPED_TRACE(std::string(c.name) + " = " + c.cbor);
            std::vector<uint8_t> request = fromHex("07 65 5479706573 A1"); /* =Types { */
            auto name = cborString(c.name);
            request.insert(request.end(), name.begin(), name.end());
            auto value = fromHex(c.cbor);
            request.insert(request.end(), value.begin(), value.end());
            EXPECT_TRUE(binary(request, fromHex("84 F6 F6")));

            EXPECT_TRUE(text(std::string("?Types/") + c.name, std::string(":85 ") + c.json));
        }
    }
};

TEST_F(ServerSerde, TextUpdateBinaryGetUnsigned)
{
    textUpdateBinaryGet({
        { "wU8", "0", "00" },
        { "wU8", "23", "17" },
        { "wU8", "24", "18 18" },
        { "wU8", "255", "18 FF" },
        { "wU16", "0", "00" },
        { "wU16", "255", "18 FF" },
        { "wU16", "256", "19 01 00" },
        { "wU16", "65535", "19 FF FF" },
        { "wU32", "0", "00" },
        { "wU32", "65535", "19 FF FF" },
        { "wU32", "65536", "1A 00 01 00 00" },
        { "wU32", "4294967295", "1A FF FF FF FF" },
        { "wU64", "4294967295", "1A FF FF FF FF" },
        { "wU64", "4294967296", "1B 00 00 00 01 00 00 00 00" },
        { "wU64", "9223372036854775807", "1B 7F FF FF FF FF FF FF FF" },
    });
}

TEST_F(ServerSerde, TextUpdateBinaryGetSigned)
{
    textUpdateBinaryGet({
        { "wI8", "0", "00" },
        { "wI8", "127", "18 7F" },
        { "wI8", "-0", "00" },
        { "wI8", "-24", "37" },
        { "wI8", "-25", "38 18" },
        { "wI8", "-128", "38 7F" },
        { "wI16", "32767", "19 7F FF" },
        { "wI16", "-256", "38 FF" },
        { "wI16", "-257", "39 01 00" },
        { "wI16", "-32768", "39 7F FF" },
        { "wI32", "2147483647", "1A 7F FF FF FF" },
        { "wI32", "-65536", "39 FF FF" },
        { "wI32", "-65537", "3A 00 01 00 00" },
        { "wI32", "-2147483648", "3A 7F FF FF FF" },
        { "wI64", "4294967296", "1B 00 00 00 01 00 00 00 00" },
        { "wI64", "9223372036854775807", "1B 7F FF FF FF FF FF FF FF" },
        { "wI64", "-4294967296", "3A FF FF FF FF" },
        { "wI64", "-4294967297", "3B 00 00 00 01 00 00 00 00" },
        { "wI64", "-9223372036854775808", "3B 7F FF FF FF FF FF FF FF" },
    });
}

TEST_F(ServerSerde, TextUpdateBinaryGetFloat)
{
    textUpdateBinaryGet({
        { "wF32", "12.340", "FA 41 45 70 A4" },
        { "wF32", "-12.340", "FA C1 45 70 A4" },
        { "wF32", "12.345", "FA 41 45 85 1F" },
    });
}

TEST_F(ServerSerde, TextUpdateBinaryGetBool)
{
    textUpdateBinaryGet({
        { "wBool", "true", "F5" },
        { "wBool", "false", "F4" },
    });
}

TEST_F(ServerSerde, TextUpdateBinaryGetString)
{
    textUpdateBinaryGet({
        { "wString", "\"Test\"", "64 54 65 73 74" },
        { "wString", "\"Hello World!\"", "6C 48 65 6C 6C 6F 20 57 6F 72 6C 64 21" },
    });
}

TEST_F(ServerSerde, BinaryUpdateTextGetUnsigned)
{
    binaryUpdateTextGet({
        { "wU8", "0", "00" },
        { "wU8", "23", "17" },
        { "wU8", "24", "18 18" },
        { "wU8", "255", "18 FF" },
        { "wU16", "255", "19 00 FF" }, // not the shortest encoding
        { "wU16", "256", "19 01 00" },
        { "wU16", "65535", "19 FF FF" },
        { "wU32", "255", "19 00 FF" },
        { "wU32", "65535", "1A 00 00 FF FF" },
        { "wU32", "65536", "1A 00 01 00 00" },
        { "wU32", "4294967295", "1A FF FF FF FF" },
        { "wU64", "4294967295", "1B 00 00 00 00 FF FF FF FF" },
        { "wU64", "4294967296", "1B 00 00 00 01 00 00 00 00" },
        { "wU64", "18446744073709551615", "1B FF FF FF FF FF FF FF FF" },
    });
}

TEST_F(ServerSerde, BinaryUpdateTextGetSigned)
{
    binaryUpdateTextGet({
        { "wI8", "23", "17" },
        { "wI8", "127", "18 7F" },
        { "wI8", "-24", "37" },
        { "wI8", "-25", "38 18" },
        { "wI8", "-128", "38 7F" },
        { "wI16", "255", "19 00 FF" },
        { "wI16", "32767", "19 7F FF" },
        { "wI16", "-257", "39 01 00" },
        { "wI16", "-32768", "39 7F FF" },
        { "wI32", "65535", "1A 00 00 FF FF" },
        { "wI32", "2147483647", "1A 7F FF FF FF" },
        { "wI32", "-65537", "3A 00 01 00 00" },
        { "wI32", "-2147483648", "3A 7F FF FF FF" },
        { "wI64", "4294967296", "1B 00 00 00 01 00 00 00 00" },
        { "wI64", "9223372036854775807", "1B 7F FF FF FF FF FF FF FF" },
        { "wI64", "-4294967297", "3B 00 00 00 01 00 00 00 00" },
        { "wI64", "-9223372036854775808", "3B 7F FF FF FF FF FF FF FF" },
    });
}

/// Disabled: thingset++ currently always writes six decimal places: 12.340000.
TEST_F(ServerSerde, DISABLED_BinaryUpdateTextGetFloat)
{
    binaryUpdateTextGet({
        { "wF32", "12.34", "FA 41 45 70 A4" },
        { "wF32", "-12.34", "FA C1 45 70 A4" },
    });
}

TEST_F(ServerSerde, BinaryUpdateTextGetBool)
{
    binaryUpdateTextGet({
        { "wBool", "true", "F5" },
        { "wBool", "false", "F4" },
    });
}

TEST_F(ServerSerde, BinaryUpdateTextGetString)
{
    binaryUpdateTextGet({
        { "wString", "\"Test\"", "64 54 65 73 74" },
        { "wString", "\"Hello World!\"", "6C 48 65 6C 6C 6F 20 57 6F 72 6C 64 21" },
    });
}

