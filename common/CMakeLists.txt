add_library(common_shared STATIC
        renderer/Renderer.cpp
        window/Window.cpp
        ui/backend/UIBackend.cpp
        application/Application.cpp
        message/Message.cpp
)

target_include_directories(common_shared PUBLIC
        .
        application
        protocol
        window
        renderer
        ui
        utility
)

target_link_libraries(common_shared
        PUBLIC
        asio_lib
        json_lib
        imgui_lib
        glfw
        Vulkan::Vulkan
)