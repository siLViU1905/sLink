#ifndef SLINK_CLIENT_APPLICATION_H
#define SLINK_CLIENT_APPLICATION_H

#include "../client/Client.h"
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
        asio::io_context m_IOContext;

        client::Client m_Client;

        std::jthread m_NetworkThread;
    };
}

#endif
