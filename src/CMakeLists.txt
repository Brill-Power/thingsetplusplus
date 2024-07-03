target_sources(thingset++ PRIVATE ThingSetBinaryDecoder.cpp)
target_sources(thingset++ PRIVATE ThingSetBinaryEncoder.cpp)

if(ENABLE_SERVER)
    target_sources(thingset++ PRIVATE ThingSetParentNode.cpp)
    target_sources(thingset++ PRIVATE ThingSetRegistry.cpp)
    target_sources(thingset++ PRIVATE ThingSetServer.cpp)
endif()

if(ENABLE_CLIENT)
    target_sources(thingset++ PRIVATE ThingSetClient.cpp)
endif()

if(ENABLE_ASIO)
    target_sources(thingset++ PRIVATE asio/ThingSetAsyncSocketServerTransport.cpp)
    target_sources(thingset++ PRIVATE asio/ThingSetAsyncSocketClientTransport.cpp)
endif()