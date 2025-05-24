if(ENABLE_ASIO)
    message("ASIO IP support enabled")
    add_subdirectory(asio)
endif()

if (ENABLE_SOCKETS)
    message("Zephyr Socket IP support enabled")
    add_subdirectory(sockets)
endif()

if (NOT ENABLE_ZEPHYR)
    target_sources(thingset++ PRIVATE InterfaceInfo.cpp)
endif()
