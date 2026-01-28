#ifndef SLINK_PACKET_H
#define SLINK_PACKET_H
#include "packet_header.h"
#include <vector>

namespace slink::protocol
{
    struct Packet
    {
        PacketHeader header;

        std::vector<std::byte> data;
    };
}

#endif
