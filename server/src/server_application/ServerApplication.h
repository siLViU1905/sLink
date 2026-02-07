#ifndef SLINK_SERVER_APPLICATION_H
#define SLINK_SERVER_APPLICATION_H

#include "../server/Server.h"
#include <application/Application.h>
#include <layers/UILayer.h>

namespace sLink::server_application
{
    class ServerApplication : public application::Application
    {
    public:
        ServerApplication(int windowWidth, int windowHeight, std::string_view windowName);

    protected:
        void onUpdate() override;

        void onRender() override;

        void onRenderUI() override;

    private:
        asio::io_context m_IOContext;

        server::Server m_Server;

        std::jthread m_NetworkThread;

        ui::layer::UILayer m_MainLayer;

        void initLayers();
      
    };
}

#endif
