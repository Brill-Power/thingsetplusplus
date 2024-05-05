cmake_minimum_required(VERSION 3.13.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(thingset_test LANGUAGES C CXX VERSION 1.0.0)

add_subdirectory(../googletest)

add_executable(testapp)
target_sources(testapp PRIVATE TestBinaryEncoder.cpp)
target_link_libraries(testapp PRIVATE gtest_main)

enable_testing()