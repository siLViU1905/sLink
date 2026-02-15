#ifndef SLINK_COMMAND_H
#define SLINK_COMMAND_H
#include <cstdint>

namespace sLink::protocol
{
    enum class Command : uint8_t
    {
        LOGIN_REQUEST,
        LOGIN_RESPONSE,
        CHAT_MESSAGE,
        USER_JOINED,
        USER_LEFT
    };
}

#endif