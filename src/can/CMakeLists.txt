target_sources(thingset++ PRIVATE CanID.cpp
    ThingSetCanInterface.cpp
    ThingSetCanClientTransport.cpp
    ThingSetCanServerTransport.cpp
    ThingSetCanSubscriptionTransport.cpp
    StreamingCanThingSetBinaryEncoder.cpp)

if(ENABLE_SOCKETCAN)
    message("SocketCAN support enabled")
    add_subdirectory(socketcan)
endif()

if(ENABLE_ZEPHYR)
    message("Zephyr support enabled")
    add_subdirectory(zephyr)
endif()