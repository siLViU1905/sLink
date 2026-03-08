add_executable(sLink_Client
        src/main.cpp
        src/client_application/ClientApplication.cpp
        src/client/Client.cpp
        src/ui/components/chat_window/UIChatWindow.cpp
        src/ui/components/client_login/UIClientLogin.cpp
        src/ui/layers/chat_layer/UIChatLayer.cpp
        src/ui/layers/login_layer/UILoginLayer.cpp
        src/ui/components/client_register/UIClientRegister.cpp
        src/ui/layers/register_layer/UIRegisterLayer.cpp
)

target_link_libraries(sLink_Client PRIVATE
        common_shared
)

# Copy assets folder to executable directory after build
add_custom_command(TARGET sLink_Client POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:sLink_Client>/assets
    COMMENT "Copying assets to client executable directory"
)