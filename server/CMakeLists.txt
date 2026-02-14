add_executable(sLink_Server
        src/main.cpp
        src/server_application/ServerApplication.cpp
        src/server/Server.cpp
        src/session/Session.cpp
        src/ui/components/active_clients/UIActiveClients.cpp
        src/ui/components/client_logging/UIClientLogging.cpp
        src/ui/layers/clients_layer/UIClientsLayer.cpp
        src/ui/components/port_select/UIPortSelect.cpp
        src/ui/layers/server_port/UIServerPortLayer.cpp
        src/database/Database.cpp
)

target_link_libraries(sLink_Server
        PRIVATE
        common_shared
        sqlite3_lib
)

set_target_properties(sLink_Server PROPERTIES OUTPUT_NAME "sLinkServer")