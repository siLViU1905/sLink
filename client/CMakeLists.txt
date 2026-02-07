add_executable(sLink_Client
        src/main.cpp
        src/client_application/ClientApplication.cpp
        src/client/Client.cpp
)

target_link_libraries(sLink_Client PRIVATE common_shared)