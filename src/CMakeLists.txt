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
    target_sources(thingset++ PRIVATE ThingSetClient.cpp)
endif()

if(ENABLE_ASIO)
    add_subdirectory(ip)
endif()

if(ENABLE_SOCKETCAN)
    add_subdirectory(can)
endif()

if(ENABLE_ZEPHYR)
    add_subdirectory(can)
    target_sources(thingset++ PRIVATE ZephyrLogRegistration.cpp)
endif()