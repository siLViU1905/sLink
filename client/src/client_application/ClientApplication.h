#ifndef SLINK_CLIENT_APPLICATION_H
#define SLINK_CLIENT_APPLICATION_H

#include <application/Application.h>

namespace sLink::client_application
{
    class ClientApplication : public application::Application
    {
    public:
        ClientApplication(int windowWidth, int windowHeight, std::string_view windowName);

    protected:
        void onUpdate() override;

        void onRender() override;

    private:
      
    };
}

#endif
