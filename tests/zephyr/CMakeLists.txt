cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(thingset_plus_plus_zephyr_test)

set(CMAKE_CXX_STANDARD 20)

FILE(GLOB test_sources src/*.cpp)
target_sources(app PRIVATE ${test_sources})
