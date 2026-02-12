add_executable(sLink_Server
        src/main.cpp
        src/server_application/ServerApplication.cpp
        src/server/Server.cpp
        src/session/Session.cpp
        src/ui/components/active_clients/UIActiveClients.cpp
        src/ui/components/client_logging/UIClientLogging.cpp
        src/ui/layers/clients_layer/UIClientsLayer.cpp
)

target_link_libraries(sLink_Server
        PRIVATE
        common_shared
)

set_target_properties(sLink_Server PROPERTIES OUTPUT_NAME "sLinkServer")