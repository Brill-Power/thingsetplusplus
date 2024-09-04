if(ENABLE_SOCKETCAN)
    target_sources(thingset++ PRIVATE CanFrame.cpp
        RawCanSocket.cpp
        IsoTpCanSocket.cpp
        ThingSetSocketCanInterface.cpp
        ThingSetSocketCanServerTransport.cpp)
endif()