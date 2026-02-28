# sLink

A real-time local messaging application built on a C++23 stack, combining a Vulkan rendering engine with an Asio networking layer.

Here is a demo: https://github.com/user-attachments/assets/9c04ea7e-84c3-4944-aad5-a477dbafe363

## Technologies

| Area | Library / Tool |
|---|---|
| Language | C++23 |
| Graphics | Vulkan RAII 1.4 |
| UI | ImGui |
| Networking | Asio (standalone) |
| Serialization | nlohmann/json |
| Storage | SQLite |
| Build System | CMake |

## Architecture

The project is split into three modules:

- **common** — Shared static library containing the communication protocol, message system, and the Vulkan/ImGui rendering engine.
- **client** — End-user chat application.
- **server** — Administration tool for hosting a session and monitoring active users in real time.

## Requirements

- Vulkan SDK 1.4+
- C++23 compatible compiler
