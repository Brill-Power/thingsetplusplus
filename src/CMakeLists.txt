target_sources(thingset++ PRIVATE Eui.cpp
    ThingSetBinaryDecoder.cpp
    ThingSetBinaryEncoder.cpp
    ThingSetNode.cpp
    ThingSetParentNode.cpp
    ThingSetRegistry.cpp)

if(ENABLE_SERVER)
    target_sources(thingset++ PRIVATE ThingSetRequestContext.cpp
        ThingSetServer.cpp)
endif()

if(ENABLE_CLIENT)
    target_sources(thingset++ PRIVATE ThingSetClient.cpp)
endif()

if(ENABLE_ASIO)
    add_subdirectory(ip)
endif()

if (ENABLE_CAN OR ENABLE_SOCKETCAN)
    add_subdirectory(can)
endif()

if (ENABLE_SOCKETS)
    add_subdirectory(ip)
endif()

if(ENABLE_ZEPHYR)
    target_sources(thingset++ PRIVATE ZephyrLogRegistration.cpp)
endif()
