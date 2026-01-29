cmake_minimum_required(VERSION 3.28)

project(sLink
        VERSION 1.0
        LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#if your vulkan version is different specify it here
set(VULKAN_SDK "C:/VulkanSDK/1.4.328.1")
set(Vulkan_ROOT "${VULKAN_SDK}")
set(Vulkan_INCLUDE_DIR "${VULKAN_SDK}/Include")

set(GLFW_PATH "${CMAKE_SOURCE_DIR}/external/glfw")

find_package(Vulkan 1.4 REQUIRED)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(${GLFW_PATH})

add_subdirectory(external)

add_subdirectory(common)

add_subdirectory(client)
add_subdirectory(server)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)