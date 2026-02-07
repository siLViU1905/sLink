add_library(asio_lib INTERFACE)

target_include_directories(asio_lib INTERFACE asio/include)
target_compile_definitions(asio_lib INTERFACE 
        ASIO_STANDALONE
        $<$<PLATFORM_ID:Windows>:_WIN32_WINNT=0x0601>
)

add_library(imgui_lib STATIC
        imgui/imgui.cpp
        imgui/imgui_draw.cpp
        imgui/imgui_widgets.cpp
        imgui/imgui_tables.cpp
        imgui/backends/imgui_impl_vulkan.cpp
        imgui/backends/imgui_impl_glfw.cpp
)

target_include_directories(imgui_lib PUBLIC imgui imgui/backends)

target_link_libraries(imgui_lib PRIVATE
        Vulkan::Vulkan
        glfw
)