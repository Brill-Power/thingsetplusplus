if(ENABLE_ASIO)
    message("ASIO UDP support enabled")
    add_subdirectory(asio)
endif()
