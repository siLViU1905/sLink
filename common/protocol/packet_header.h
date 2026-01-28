#ifndef SLINK_PACKET_HEADER_H
#define SLINK_PACKET_HEADER_H
#include "command.h"

namespace slink::protocol
{
    struct PacketHeader
    {
        Command command;

        size_t payloadSize;
    };
}

#endif