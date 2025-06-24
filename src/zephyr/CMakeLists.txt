if (ENABLE_ZEPHYR AND CONFIG_THINGSET_PLUS_PLUS_BACKEND_SHELL)
    target_sources(thingset++ PRIVATE
        ThingSetZephyrShellServerTransport.cpp
        StreamingZephyrShellThingSetBinaryEncoder.cpp)
endif()
