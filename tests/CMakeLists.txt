cmake_minimum_required(VERSION 3.13.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_CXX_FLAGS -O3)
set(ENABLE_ASIO ON)
set(ENABLE_SOCKETS ON)
set(ENABLE_SERVER ON)
set(ENABLE_CLIENT ON)
set(DEBUG_LOGGING ON)

project(thingset_test LANGUAGES C CXX VERSION 1.0.0)

add_subdirectory(../googletest googletest)
add_subdirectory(../ thingset++)

add_executable(testapp)
include_directories(../include ../zcbor/include ../asio/asio/include)

target_sources(testapp PRIVATE TestBinaryEncoder.cpp
    TestBinaryDecoder.cpp
    TestTextEncoder.cpp
    TestCompatibility.cpp
    TestProperties.cpp
    TestFunctions.cpp
    TestAsioIpClientServer.cpp)
# figure out how to get this to compile with Clang
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_sources(testapp PRIVATE TestBinaryEncodingRecords.cpp)
    target_sources(testapp PRIVATE TestTextEncodingRecords.cpp)
    target_sources(testapp PRIVATE TestBinaryDecodingRecords.cpp)
endif()
# regrettably exlcude this test until we figure out why Socket server is broken on macOS
if(NOT APPLE)
    target_sources(testapp PRIVATE TestSocketIpClientServer.cpp)
endif()

target_link_libraries(testapp PRIVATE thingset++)
target_link_libraries(testapp PRIVATE gtest_main)

enable_testing()