if(ENABLE_SOCKETCAN)
    target_sources(thingset++ PRIVATE CanFrame.cpp
        RawCanSocket.cpp
        RawCanSocketListener.cpp
        IsoTpCanSocket.cpp
        ThingSetSocketCanInterface.cpp
        ThingSetSocketCanClientTransport.cpp
        ThingSetSocketCanServerTransport.cpp
        ThingSetSocketCanSubscriptionTransport.cpp)
    
    # Allow CAN address range to be configured for SocketCAN builds
    if(NOT DEFINED THINGSET_CAN_MIN_ADDRESS)
        set(THINGSET_CAN_MIN_ADDRESS 0x01)
    endif()
    if(NOT DEFINED THINGSET_CAN_MAX_ADDRESS)
        set(THINGSET_CAN_MAX_ADDRESS 0xfd)
    endif()
    
    target_compile_definitions(thingset++ PRIVATE
        THINGSET_CAN_MIN_ADDRESS=${THINGSET_CAN_MIN_ADDRESS}
        THINGSET_CAN_MAX_ADDRESS=${THINGSET_CAN_MAX_ADDRESS})
endif()