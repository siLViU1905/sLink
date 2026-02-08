add_executable(sLink_Server
        src/main.cpp
        src/server_application/ServerApplication.cpp
        src/server/Server.cpp
        src/session/Session.cpp
)

target_link_libraries(sLink_Server
        PRIVATE
        common_shared
)

set_target_properties(sLink_Server PROPERTIES OUTPUT_NAME "sLinkServer")