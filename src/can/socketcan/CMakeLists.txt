if(ENABLE_SOCKETCAN)
    target_sources(thingset++ PRIVATE CanFrame.cpp
        RawCanSocket.cpp
        RawCanSocketListener.cpp
        IsoTpCanSocket.cpp
        ThingSetSocketCanInterface.cpp
        ThingSetSocketCanClientTransport.cpp
        ThingSetSocketCanServerTransport.cpp
        ThingSetSocketCanSubscriptionTransport.cpp)
endif()