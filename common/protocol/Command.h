#ifndef SLINK_COMMAND_H
#define SLINK_COMMAND_H
#include <cstdint>

namespace sLink::protocol
{
    enum class Command : uint8_t
    {
        LOGIN_REQUEST,
        REGISTER_REQUEST,
        LOGIN_RESPONSE_REJECT,
        LOGIN_RESPONSE_ACCEPT,
        CHAT_MESSAGE,
        USER_JOINED,
        USER_LEFT,
        SERVER_KICK_REQUEST
    };
}

#endif