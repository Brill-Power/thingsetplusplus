cmake_minimum_required(VERSION 3.13.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_CXX_FLAGS -O3)
# set(ENABLE_ASIO ON)

project(eeprom LANGUAGES C CXX VERSION 1.0.0)

add_subdirectory(../../ thingset++)

add_executable(eeprom)
include_directories(../../include ../../zcbor/include)

target_sources(eeprom PRIVATE main.cpp)

target_link_libraries(eeprom PRIVATE thingset++)
