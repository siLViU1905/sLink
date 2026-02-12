#ifndef SLINK_SERVER_APPLICATION_H
#define SLINK_SERVER_APPLICATION_H

#include "../server/Server.h"
#include <application/Application.h>
#include "../ui/layers/clients_layer/UiClientsLayer.h"

namespace sLink::server_application
{
    class ServerApplication : public application::Application
    {
    public:
        ServerApplication(int windowWidth, int windowHeight, std::string_view windowName);

        ~ServerApplication();

    protected:
        void onUpdate() override;

        void onRender() override;

        void onRenderUI() override;

    private:
        void initLayers();

        void onUpdateConnectedClients();

        void onUpdateDisconnectedClients();

        asio::io_context m_IOContext;

        server::Server m_Server;

        std::jthread m_NetworkThread;

        std::shared_ptr<ui::layer::UILayer> m_CurrentLayer;

        std::shared_ptr<server::ui::layer::UIClientsLayer> m_ClientsLayer;
    };
}

#endif
