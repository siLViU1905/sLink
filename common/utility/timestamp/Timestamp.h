#ifndef SLINK_TIMESTAMP_H
#define SLINK_TIMESTAMP_H
#include <cstdint>
#include <string>

namespace sLink::utility
{
    class Timestamp
    {
    public:
        Timestamp();

        int64_t getMs() const;

        std::string toString() const;
    private:
        int64_t m_MsSinceEpoch;
    };
}

#endif //SLINK_TIMESTAMP_H
