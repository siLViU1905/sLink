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
        src/user/User.cpp
        src/database/password_hasher/PasswordHasher.cpp
)

target_link_libraries(sLink_Server
        PRIVATE
        common_shared
        sqlite3_lib
)

set_target_properties(sLink_Server PROPERTIES OUTPUT_NAME "sLinkServer")

# Copy assets folder to executable directory after build
add_custom_command(TARGET sLink_Server POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:sLink_Server>/assets
    COMMENT "Copying assets to server executable directory"
)