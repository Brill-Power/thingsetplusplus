zephyr_library_named("thingset++")

set(CMAKE_CXX_STANDARD 20)

get_filename_component(THINGSET_BASE ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)

zephyr_include_directories(${THINGSET_BASE}/include)

set(ENABLE_ZEPHYR ON)

if(DEFINED CONFIG_THINGSET_PLUS_PLUS_SERVER)
    message("ThingSet++ server enabled")
    set(ENABLE_SERVER ON)
endif()

if(DEFINED CONFIG_THINGSET_PLUS_PLUS_CLIENT)
    message("ThingSet++ client enabled")
    set(ENABLE_CLIENT ON)
endif()


if (DEFINED CONFIG_THINGSET_PLUS_PLUS_BACKEND_SOCKET)
    set(ENABLE_ZSOCK ON)
endif()

add_subdirectory_ifdef(CONFIG_THINGSET_PLUS_PLUS ${THINGSET_BASE}/src build)