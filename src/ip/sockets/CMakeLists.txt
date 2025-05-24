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