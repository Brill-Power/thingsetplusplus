cmake_minimum_required(VERSION 3.13.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_CXX_FLAGS -O3)

project(thingset_test LANGUAGES C CXX VERSION 1.0.0)

add_subdirectory(../googletest googletest)
add_subdirectory(../ thingset++)

add_executable(testapp)
include_directories(../include ../zcbor/include)

target_sources(testapp PRIVATE TestBinaryEncoder.cpp
    TestBinaryDecoder.cpp
    TestCompatibility.cpp
    TestProperties.cpp
    TestFunctions.cpp)
# figure out how to get this to compile with Clang
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_sources(testapp PRIVATE TestRecords.cpp)
endif()

target_link_libraries(testapp PRIVATE thingset++)
target_link_libraries(testapp PRIVATE gtest_main)

enable_testing()