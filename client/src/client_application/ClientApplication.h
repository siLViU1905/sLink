#ifndef SLINK_CLIENT_APPLICATION_H
#define SLINK_CLIENT_APPLICATION_H

#include "../client/Client.h"
#include <application/Application.h>
#include  "../ui/layers/chat_layer/UIChatLayer.h"
#include "../ui/layers/login_layer/UILoginLayer.h"
#include "../ui/layers/register_layer/UIRegisterLayer.h"
#include <utility/file_explorer/FileExplorer.h>

namespace sLink::client_application
{
    class ClientApplication : public application::Application
    {
    public:
        ClientApplication(int windowWidth, int windowHeight, std::string_view windowName);

        ~ClientApplication();

    protected:
        void onUpdate() override;

        void onRender() override;

        void onRenderUI() override;

    private:
        void initLayers();

        void onConnect(std::string_view username, std::string_view password, std::string_view serverPort, protocol::Command joinType);

        void handleProfilePictureCreation(std::string_view username, const profile_picture::ProfilePicture &profilePicture);

        asio::io_context m_IOContext;

        client::Client m_Client;

        utility::FileExplorer m_FileExplorer;

        std::jthread m_NetworkThread;

        std::shared_ptr<ui::layer::UILayer> m_CurrentLayer;

        std::shared_ptr<client::ui::layer::UIChatLayer> m_ChatLayer;

        std::shared_ptr<client::ui::layer::UILoginLayer> m_LoginLayer;

        std::shared_ptr<client::ui::layer::UIRegisterLayer> m_RegisterLayer;
    };
}

#endif