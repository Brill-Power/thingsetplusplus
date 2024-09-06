target_sources(thingset++ PRIVATE CanID.cpp
    ThingSetCanInterface.cpp
    ThingSetCanServerTransport.cpp)

if(ENABLE_SOCKETCAN)
    target_sources(thingset++ PRIVATE StreamingThingSetBinaryEncoder.cpp)
    add_subdirectory(socketcan)
endif()