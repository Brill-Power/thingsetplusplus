target_sources(thingset++ PRIVATE CanFrame.cpp
    ThingSetZephyrCanInterface.cpp
    ThingSetZephyrCanServerTransport.cpp)

add_subdirectory(isotp)