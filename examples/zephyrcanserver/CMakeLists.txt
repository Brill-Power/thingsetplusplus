cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(zephyrcanserver)

set(CMAKE_CXX_STANDARD 20)

FILE(GLOB app_sources *.cpp)
target_sources(app PRIVATE ${app_sources})
