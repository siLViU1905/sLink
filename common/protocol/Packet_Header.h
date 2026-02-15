#ifndef SLINK_PACKET_HEADER_H
#define SLINK_PACKET_HEADER_H
#include "Command.h"

namespace sLink::protocol
{
    struct PacketHeader
    {
        Command m_Command;

        size_t m_PayloadSize;
    };
}

#endif