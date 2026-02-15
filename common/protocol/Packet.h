#ifndef SLINK_PACKET_H
#define SLINK_PACKET_H
#include "Packet_Header.h"
#include <vector>

namespace sLink::protocol
{
    struct Packet
    {
        PacketHeader m_Header;

        std::vector<std::byte> m_Data;
    };
}

#endif