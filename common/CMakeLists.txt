add_library(common_shared STATIC
        renderer/Renderer.cpp
)

target_include_directories(common_shared PUBLIC
        .
        protocol
        renderer
        ui
)

target_link_libraries(common_shared
        PUBLIC
        asio_lib
        imgui_lib
        Vulkan::Vulkan
)