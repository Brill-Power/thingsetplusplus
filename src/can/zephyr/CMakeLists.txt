target_sources(thingset++ PRIVATE CanFrame.cpp
    ThingSetZephyrCanInterface.cpp
    ThingSetZephyrCanClientTransport.cpp
    ThingSetZephyrCanServerTransport.cpp
    ThingSetZephyrCanSubscriptionTransport.cpp
    ThingSetZephyrCanRequestResponseContext.cpp)

add_subdirectory(isotp)