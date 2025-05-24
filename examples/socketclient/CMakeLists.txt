cmake_minimum_required(VERSION 3.13.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_CXX_FLAGS -O3)
set(ENABLE_SOCKETS ON)
set(ENABLE_CLIENT ON)

project(socketclient LANGUAGES C CXX VERSION 1.0.0)

add_subdirectory(../../ thingset++)

add_executable(socketclient)
include_directories(../../include ../../zcbor/include)

target_sources(socketclient PRIVATE main.cpp)

target_link_libraries(socketclient PRIVATE thingset++)
