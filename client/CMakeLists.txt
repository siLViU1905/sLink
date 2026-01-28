add_executable(sLink_Client
        src/main.cpp
)

target_link_libraries(sLink_Client PRIVATE common_shared)