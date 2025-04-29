if(ENABLE_ASIO)
    message("ASIO IP support enabled")
    add_subdirectory(asio)
endif()

if (ENABLE_ZSOCK)
    message("Zephyr Socket IP support enabled")
    add_subdirectory(zsock)
endif()
