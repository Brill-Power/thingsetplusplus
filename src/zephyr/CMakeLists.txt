if (ENABLE_ZEPHYR)
    if(CONFIG_THINGSET_PLUS_PLUS_BACKEND_SHELL)
        target_sources(thingset++ PRIVATE
            ThingSetZephyrShellServerTransport.cpp
            StreamingZephyrShellThingSetBinaryEncoder.cpp)
    endif()
    if(CONFIG_THINGSET_PLUS_PLUS_EEPROM)
        target_sources(thingset++ PRIVATE ThingSetEeprom.cpp)
    endif()
endif()
