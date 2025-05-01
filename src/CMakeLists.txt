target_sources(thingset++ PRIVATE Eui.cpp
    ThingSetBinaryDecoder.cpp
    ThingSetBinaryEncoder.cpp)

if(ENABLE_SERVER)
    target_sources(thingset++ PRIVATE ThingSetNode.cpp
        ThingSetParentNode.cpp
        ThingSetRegistry.cpp
        ThingSetRequestContext.cpp
        ThingSetServer.cpp)
endif()

if(ENABLE_CLIENT)
    message("WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW")
    target_sources(thingset++ PRIVATE ThingSetClient.cpp ThingSetRegistry.cpp ThingSetParentNode.cpp)
endif()

if(ENABLE_ASIO)
    target_sources(thingset++ PRIVATE asio/ThingSetAsyncSocketServerTransport.cpp
        asio/ThingSetAsyncSocketClientTransport.cpp)
endif()

if (ENABLE_CAN)
    add_subdirectory(can)
endif()

if (ENABLE_ZSOCK)
    target_sources(thingset++ PRIVATE zsock/ThingSetZephyrSocketClientTransport.cpp)
endif()
