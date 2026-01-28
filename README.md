sLink | Local Chat App

sLink is a real-time messaging application. It leverages a C++23 stack to combine graphics with a networking infrastructure.

🚀 Key Technologies
Language: C++23

Graphics: Vulkan RAII 1.4

UI: ImGui

Networking: Asio

Build System: CMake

🏗️ Project Architecture
The project is organized into three distinct modules to ensure a clean separation of concerns:

common/: The core shared library containing the communication protocol, and the shared Vulkan/ImGui rendering engine.

client/: The end-user application

server/: An administration tool that allows real-time monitoring of active users.

📋 Requirements
Vulkan SDK 1.4+

C++23 Compatible Compiler