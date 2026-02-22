add_executable(sLink_Client
        src/main.cpp
        src/client_application/ClientApplication.cpp
        src/client/Client.cpp
        src/ui/components/chat_window/UIChatWindow.cpp
        src/ui/components/client_login/UIClientLogin.cpp
        src/ui/layers/chat_layer/UIChatLayer.cpp
        src/ui/layers/login_layer/UILoginLayer.cpp
        src/ui/components/client_register/UIClientRegister.cpp
)

target_link_libraries(sLink_Client PRIVATE common_shared)