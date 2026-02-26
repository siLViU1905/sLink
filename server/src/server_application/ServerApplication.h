#ifndef SLINK_SERVER_APPLICATION_H
#define SLINK_SERVER_APPLICATION_H

#include "../server/Server.h"
#include <application/Application.h>

#include "../database/Database.h"
#include "../ui/layers/clients_layer/UiClientsLayer.h"
#include "../ui/layers/server_port/UIServerPortLayer.h"

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

        void onPortSelected(std::string_view port);

        void onUpdateConnectedClients();

        void onConnectedClientsAction(server::ui::component::UIActiveClients::Action action, std::string_view username);

        void onUpdateDisconnectedClients();

        void onUpdateDbInfo();

        asio::io_context m_IOContext;

        server::Server m_Server;

        std::jthread m_NetworkThread;

        server::db::Database m_Database;

        std::jthread m_DbThread;

        std::shared_ptr<ui::layer::UILayer> m_CurrentLayer;

        std::shared_ptr<server::ui::layer::UIClientsLayer> m_ClientsLayer;

        std::shared_ptr<server::ui::layer::UIServerPortLayer> m_ServerPortLayer;
    };
}

#endif