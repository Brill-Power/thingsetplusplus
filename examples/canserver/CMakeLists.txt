cmake_minimum_required(VERSION 3.13.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_CXX_FLAGS -O3)
set(ENABLE_SOCKETCAN ON)

project(canserver LANGUAGES C CXX VERSION 1.0.0)

add_compile_definitions(DEBUG_LOGGING)

add_subdirectory(../../ thingset++)

add_executable(canserver)
include_directories(include ../../include ../../zcbor/include)

target_sources(canserver PRIVATE main.cpp)

target_link_libraries(canserver PRIVATE thingset++)
