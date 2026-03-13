add_library(common_shared STATIC
        renderer/Renderer.cpp
        window/Window.cpp
        ${CMAKE_SOURCE_DIR}/common/imgui_bindings/imgui_impl_glfw.cpp
        ${CMAKE_SOURCE_DIR}/common/imgui_bindings/imgui_impl_vulkan.cpp
        ui/backend/UIBackend.cpp
        application/Application.cpp
        message/Message.cpp
        utility/timestamp/Timestamp.cpp
        ui/components/info/UIInfo.cpp
        profile_picture/ProfilePicture.cpp
        utility/file_explorer/FileExplorer.cpp
        utility/base64/Base64.cpp
        sound/Sound.cpp
)

target_include_directories(common_shared PUBLIC
        .
        application
        protocol
        window
        renderer
        ui
        utility
        ${CMAKE_SOURCE_DIR}/common/imgui_bindings
)

target_link_libraries(common_shared
        PUBLIC
        asio::asio
        nlohmann_json::nlohmann_json
        imgui::imgui
        stb::stb
        miniaudio::miniaudio
        glfw
        Vulkan::Vulkan
)