target_sources(thingset++ PRIVATE Eui.cpp
    ThingSetBinaryDecoder.cpp
    ThingSetBinaryEncoder.cpp
    ThingSetEncoder.cpp
    ThingSetDecoder.cpp
    ThingSetNode.cpp
    ThingSetParentNode.cpp
    ThingSetRegistry.cpp
    ThingSetTextDecoder.cpp
    ThingSetTextEncoder.cpp)

if(ENABLE_SERVER)
    target_sources(thingset++ PRIVATE ThingSetRequestContext.cpp
        ThingSetServer.cpp)
endif()

if(ENABLE_CLIENT)
    target_sources(thingset++ PRIVATE ThingSetClient.cpp)
endif()

if(ENABLE_ASIO OR ENABLE_SOCKETS)
    add_subdirectory(ip)
endif()

if (ENABLE_CAN OR ENABLE_SOCKETCAN)
    add_subdirectory(can)
endif()

if(ENABLE_ZEPHYR)
    target_sources(thingset++ PRIVATE ZephyrLogRegistration.cpp)
    add_subdirectory(zephyr)
endif()
