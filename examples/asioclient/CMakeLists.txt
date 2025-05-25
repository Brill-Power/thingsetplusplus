cmake_minimum_required(VERSION 3.13.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_CXX_FLAGS -O3)
set(ENABLE_ASIO ON)
set(ENABLE_CLIENT ON)

project(asioclient LANGUAGES C CXX VERSION 1.0.0)

add_subdirectory(../../ thingset++)

add_executable(asioclient)
include_directories(../../include ../../zcbor/include ../../asio/asio/include)

target_sources(asioclient PRIVATE main.cpp)

target_link_libraries(asioclient PRIVATE thingset++)
