target_sources(thingset++ PRIVATE CanID.cpp
    ThingSetCanInterface.cpp
    ThingSetCanServerTransport.cpp
    StreamingThingSetBinaryEncoder.cpp)

if(ENABLE_SOCKETCAN)
    add_subdirectory(socketcan)
endif()