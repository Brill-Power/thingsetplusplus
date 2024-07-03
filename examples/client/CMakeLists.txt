cmake_minimum_required(VERSION 3.13.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_CXX_FLAGS -O3)
set(ENABLE_ASIO ON)

project(client LANGUAGES C CXX VERSION 1.0.0)

add_subdirectory(../../ thingset++)

add_executable(client)
include_directories(../../include ../../zcbor/include ../../asio/asio/include)

target_sources(client PRIVATE main.cpp)

target_link_libraries(client PRIVATE thingset++)
