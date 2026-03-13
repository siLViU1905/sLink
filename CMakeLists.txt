cmake_minimum_required(VERSION 3.28)

project(sLink
        VERSION 1.0
        LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#if your vulkan version is different specify it here
set(VULKAN_SDK "C:/VulkanSDK/1.4.328.1")
set(Vulkan_ROOT "${VULKAN_SDK}")
set(Vulkan_INCLUDE_DIR "${VULKAN_SDK}/Include")

if(CMAKE_BUILD_TYPE)
    set(CONAN_GENERATORS_DIR "${CMAKE_BINARY_DIR}/build/${CMAKE_BUILD_TYPE}/generators")

    if(EXISTS "${CONAN_GENERATORS_DIR}")
        list(APPEND CMAKE_PREFIX_PATH "${CONAN_GENERATORS_DIR}")
    else()
        list(APPEND CMAKE_PREFIX_PATH "${CMAKE_BINARY_DIR}/generators")
    endif()
endif()

find_package(Vulkan 1.4 REQUIRED)
find_package(glfw3 REQUIRED)
find_package(imgui REQUIRED)
find_package(Catch2 REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(asio REQUIRED)
find_package(stb REQUIRED)
find_package(miniaudio REQUIRED)
find_package(SQLite3 REQUIRED CONFIG)

add_subdirectory(common)

add_subdirectory(client)
add_subdirectory(server)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_subdirectory(tests)