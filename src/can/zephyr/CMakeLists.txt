target_sources(thingset++ PRIVATE CanFrame.cpp
    ThingSetZephyrCanInterface.cpp
    ThingSetZephyrCanClientTransport.cpp
    ThingSetZephyrCanServerTransport.cpp)

add_subdirectory(isotp)