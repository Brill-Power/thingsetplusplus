if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "12.0")
    message(FATAL_ERROR "Insufficient gcc version (need 12.0 or later)")
  endif()
endif()

target_sources(thingset++ PRIVATE Eui.cpp
    ThingSetBinaryDecoder.cpp
    ThingSetBinaryEncoder.cpp
    ThingSetEncoder.cpp
    ThingSetDecoder.cpp
    ThingSetNode.cpp
    ThingSetParentNode.cpp
    ThingSetRegistry.cpp)

if (ENABLE_TEXT_MODE)
    target_sources(thingset++ PRIVATE ThingSetTextDecoder.cpp
        ThingSetTextEncoder.cpp)
endif()

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
