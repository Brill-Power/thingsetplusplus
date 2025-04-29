zephyr_library_named("thingset++")

get_filename_component(THINGSET_BASE ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)

zephyr_include_directories(${THINGSET_BASE}/include)

set(ENABLE_ZEPHYR ON)
set(ENABLE_ZSOCK ON)

if(DEFINED CONFIG_THINGSET_PLUS_PLUS_SERVER)
    set(ENABLE_SERVER ON)
endif()
add_subdirectory_ifdef(CONFIG_THINGSET_PLUS_PLUS ${THINGSET_BASE}/src build)