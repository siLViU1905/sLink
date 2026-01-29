add_library(common_shared STATIC
        renderer/Renderer.cpp
        window/Window.cpp
        ui/backend/UIBackend.cpp
)

target_include_directories(common_shared PUBLIC
        .
        protocol
        window
        renderer
        ui
)

target_link_libraries(common_shared
        PUBLIC
        asio_lib
        imgui_lib
        glfw
        Vulkan::Vulkan
)