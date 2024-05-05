cmake_minimum_required(VERSION 3.13.1)

set(CMAKE_CXX_STANDARD 20)

project(thingset++ LANGUAGES C CXX VERSION 1.0.0)

include_directories(include)

add_subdirectory(src)