#ifndef SLINK_COMMAND_H
#define SLINK_COMMAND_H
#include <cstdint>

namespace slink::protocol
{
    enum class Command : uint8_t
    {
        LoginRequest,
        LoginResponse,
        ChatMessage,
        Heartbeat,
        UserJoined,
        UserLeft
    };
}

#endif
