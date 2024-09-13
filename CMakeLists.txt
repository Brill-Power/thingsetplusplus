cmake_minimum_required(VERSION 3.13.1)

set(CMAKE_CXX_STANDARD 20)

project(thingset++ LANGUAGES C CXX VERSION 1.0.0)
add_library(thingset++ SHARED)

target_compile_options(thingset++ PRIVATE -Wall -Wextra -Wpedantic)

add_compile_definitions(ZCBOR_CANONICAL)
add_library(zcbor SHARED)
target_sources(zcbor PRIVATE zcbor/src/zcbor_common.c)
target_sources(zcbor PRIVATE zcbor/src/zcbor_decode.c)
target_sources(zcbor PRIVATE zcbor/src/zcbor_encode.c)
target_sources(zcbor PRIVATE zcbor/src/zcbor_print.c)
include_directories(zcbor/include asio/asio/include)
target_link_libraries(thingset++ zcbor)

include_directories(include)

set(ENABLE_CLIENT ON)
set(ENABLE_SERVER ON)

add_subdirectory(src)