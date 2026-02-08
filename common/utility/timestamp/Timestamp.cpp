#include "Timestamp.h"

#include <chrono>

namespace sLink::utility
{
    Timestamp::Timestamp()
        : m_MsSinceEpoch(
            std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).
            count())
    {
    }

    int64_t Timestamp::getMs() const
    {
        return m_MsSinceEpoch;
    }

    std::string Timestamp::toString() const
    {
        std::chrono::milliseconds dur(m_MsSinceEpoch);

        std::chrono::sys_time time_point{dur};

        return std::format("{:%d/%m/%Y %H:%M:%S}", time_point);
    }
}
