#ifndef SLINK_FILEEXPLORER_H
#define SLINK_FILEEXPLORER_H
#include "safe_queue/SafeQueue.h"

namespace sLink::utility
{
    class FileExplorer
    {
    public:
        FileExplorer();

        void open();

        SafeQueue<std::string>& getPaths();
    private:
        SafeQueue<std::string> m_Paths;

        bool m_IsOpen;

        std::mutex m_IsOpenMutex;
    };
}

#endif
