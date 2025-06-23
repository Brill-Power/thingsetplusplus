if (ENABLE_ZEPHYR)
    target_sources(thingset++ PRIVATE
        ThingSetZephyrShellServerTransport.cpp
        StreamingZephyrShellThingSetBinaryEncoder.cpp)
endif()
