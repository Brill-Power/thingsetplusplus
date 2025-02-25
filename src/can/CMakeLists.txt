target_sources(thingset++ PRIVATE CanID.cpp
    ThingSetCanInterface.cpp
    ThingSetCanClientTransport.cpp
    ThingSetCanServerTransport.cpp)

if(ENABLE_SOCKETCAN)
    message("SocketCAN support enabled")
    target_sources(thingset++ PRIVATE StreamingCanThingSetBinaryEncoder.cpp)
    add_subdirectory(socketcan)
endif()

if(ENABLE_ZEPHYR)
    message("Zephyr support enabled")
    target_sources(thingset++ PRIVATE StreamingCanThingSetBinaryEncoder.cpp)
    add_subdirectory(zephyr)
endif()