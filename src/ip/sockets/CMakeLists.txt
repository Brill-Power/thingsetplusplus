target_sources(thingset++ PRIVATE
    SocketEndpoint.cpp
)

if(ENABLE_CLIENT)
    target_sources(thingset++ PRIVATE
        ThingSetSocketClientTransport.cpp
        ThingSetSocketSubscriptionTransport.cpp
    )
endif()

if(ENABLE_SERVER)
    target_sources(thingset++ PRIVATE
        ThingSetSocketServerTransport.cpp
    )
endif()

if (NOT ENABLE_ZEPHYR)
    if(NOT DEFINED THINGSET_PLUS_PLUS_SOCKET_SERVER_TX_BUFFER_SIZE)
        set(THINGSET_PLUS_PLUS_SOCKET_SERVER_TX_BUFFER_SIZE 1024)
    endif()
    if(NOT DEFINED THINGSET_PLUS_PLUS_SOCKET_SERVER_RX_BUFFER_SIZE)
        set(THINGSET_PLUS_PLUS_SOCKET_SERVER_RX_BUFFER_SIZE 1024)
    endif()

    target_compile_definitions(thingset++ PRIVATE
        THINGSET_PLUS_PLUS_SOCKET_SERVER_TX_BUFFER_SIZE=${THINGSET_PLUS_PLUS_SOCKET_SERVER_TX_BUFFER_SIZE}
        THINGSET_PLUS_PLUS_SOCKET_SERVER_RX_BUFFER_SIZE=${THINGSET_PLUS_PLUS_SOCKET_SERVER_RX_BUFFER_SIZE})
endif()