target_sources(thingset++ PRIVATE Eui.cpp
    ThingSetBinaryDecoder.cpp
    ThingSetBinaryEncoder.cpp)

if(ENABLE_SERVER)
    target_sources(thingset++ PRIVATE ThingSetParentNode.cpp
        ThingSetRegistry.cpp
        ThingSetServer.cpp)
endif()

if(ENABLE_CLIENT)
    target_sources(thingset++ PRIVATE ThingSetClient.cpp)
endif()

if(ENABLE_ASIO)
    target_sources(thingset++ PRIVATE asio/ThingSetAsyncSocketServerTransport.cpp
        asio/ThingSetAsyncSocketClientTransport.cpp)
endif()

if(ENABLE_SOCKETCAN)
    add_subdirectory(can)
    add_subdirectory(socketcan)
endif()