cmake_minimum_required(VERSION 3.13.1)

set(CMAKE_CXX_STANDARD 20)

project(thingset++ LANGUAGES C CXX VERSION 1.0.0)
add_library(thingset++ SHARED)

target_compile_options(thingset++ PRIVATE -Wall -Wextra -Wpedantic)

add_compile_definitions(ZCBOR_CANONICAL)
add_library(zcbor SHARED)

if(BSD)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
    target_link_libraries(thingset++ Threads::Threads)
endif()

if (ENABLE_GATEWAY)
    target_compile_definitions(thingset++ PUBLIC ENABLE_GATEWAY)
endif()

if (ENABLE_TEXT_MODE)
    target_compile_definitions(thingset++ PUBLIC ENABLE_TEXT_MODE)
endif()

if (ENABLE_ENHANCED_REPORTING)
    target_compile_definitions(thingset++ PUBLIC ENABLE_ENHANCED_REPORTING)
endif()

target_sources(zcbor PRIVATE zcbor/src/zcbor_common.c
    zcbor/src/zcbor_decode.c
    zcbor/src/zcbor_encode.c
    zcbor/src/zcbor_print.c)
include_directories(zcbor/include asio/asio/include)
target_link_libraries(thingset++ zcbor)

include_directories(include)

add_subdirectory(src)
