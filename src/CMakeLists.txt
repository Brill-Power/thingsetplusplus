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
    target_sources(thingset++ PRIVATE ThingSetNode.cpp ThingSetClient.cpp ThingSetRegistry.cpp ThingSetParentNode.cpp)
endif()

if(ENABLE_ASIO)
    add_subdirectory(ip)
endif()

if (ENABLE_CAN)
    add_subdirectory(can)
endif()

if (ENABLE_ZSOCK)
    target_sources(thingset++ PRIVATE zsock/ThingSetZephyrSocketClientTransport.cpp zsock/ThingSetZephyrSocketServerTransport.cpp)
endif()
