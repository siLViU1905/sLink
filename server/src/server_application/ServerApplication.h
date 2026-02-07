#ifndef SLINK_SERVER_APPLICATION_H
#define SLINK_SERVER_APPLICATION_H

#include <application/Application.h>

namespace sLink::server_application
{
    class ServerApplication : public application::Application
    {
    public:
        ServerApplication(int windowWidth, int windowHeight, std::string_view windowName);

    protected:
        void onUpdate() override;

        void onRender() override;

    private:
      
    };
}

#endif
