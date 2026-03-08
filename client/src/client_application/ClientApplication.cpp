#include "ClientApplication.h"

#include <filesystem>

namespace sLink::client_application
{
    ClientApplication::ClientApplication(int windowWidth, int windowHeight,
                                         std::string_view windowName) : application::Application(
                                                                            windowWidth, windowHeight, windowName),
                                                                        m_Client(m_IOContext)
    {
        initLayers();

        initSounds();

        m_NetworkThread = std::jthread([this]()
        {
            m_IOContext.run();
        });
    }

    ClientApplication::~ClientApplication()
    {
        m_IOContext.stop();
    }

    void ClientApplication::onUpdate()
    {
        while (auto raw_message = m_Client.getInbox().tryPop())
        {
            auto message = message::Message::deserialize(*raw_message);

            switch (message.getCommand())
            {
                case protocol::Command::LOGIN_RESPONSE_REJECT:
                {
                    m_CurrentLayer = m_LoginLayer;

                    m_LoginLayer->getClientLoginPanel().notifyLoginFailed(message.getContent());
                    break;
                }

                case protocol::Command::LOGIN_RESPONSE_ACCEPT:
                {
                    m_CurrentLayer = m_ChatLayer;

                    m_ChatLayer->getInfoPanel().addSuccessInfo(message.getContent());
                    break;
                }

                case protocol::Command::CHAT_MESSAGE:
                {
                    m_ChatLayer->getChatWindow().addMessage(message);
                    break;
                }

                case protocol::Command::SERVER_KICK_REQUEST:
                    m_CurrentLayer = m_LoginLayer;

                    m_LoginLayer->getClientLoginPanel().notifyKick(message.getContent());
                    break;

                case protocol::Command::PROFILE_PICTURE:
                    if (message.getSenderName() != m_Client.getUsername())
                    {
                        profile_picture::ProfilePicture profilePicture;

                        profilePicture.setPixelsFromContent(message.getContent());

                        handleProfilePictureCreation(message.getSenderName(), profilePicture);
                    }
                    break;

                default:
                    break;
            }
        }

        if (auto path = m_FileExplorer.getPaths().tryPop())
        {
            auto result = m_Client.getProfilePicture().loadImage(*path);

            if (result)
            {
                m_ChatLayer->getInfoPanel().addSuccessInfo(*result);

                m_Renderer.createClientSideProfilePicture(m_Client.getProfilePicture());

                m_LoginLayer->getClientLoginPanel().setTextureID(m_Renderer.getClientSideProfilePictureTextureID());

                m_RegisterLayer->getClientRegisterPanel().setTextureID(
                    m_Renderer.getClientSideProfilePictureTextureID());
            } else
                m_ChatLayer->getInfoPanel().addFailInfo(result.error());
        }
    }

    void ClientApplication::onRender()
    {
        m_Renderer.waitForFences();

        onRenderUI();

        m_Renderer.recordUIData(ui::UIBackend::get_ui_render_data());

        m_Renderer.renderFrame();
    }

    void ClientApplication::onRenderUI()
    {
        ui::UIBackend::begin_frame();

        m_CurrentLayer->render();

        ui::UIBackend::end_frame();
    }

    void ClientApplication::initLayers()
    {
        m_ChatLayer = std::make_shared<client::ui::layer::UIChatLayer>();

        m_ChatLayer->getChatWindow().setOnMessageSend([this](std::string_view content)
        {
            m_SendPressSound.play();
            message::Message message(
                protocol::Command::CHAT_MESSAGE,
                m_Client.getUsername(),
                content
            );
            m_Client.send(message);
        });

        m_LoginLayer = std::make_shared<client::ui::layer::UILoginLayer>();

        m_LoginLayer->getClientLoginPanel().setOnLoginDataInput(
            [this](std::string_view username, std::string_view password, std::string_view serverPort)
            {
                onConnect(username, password, serverPort, protocol::Command::LOGIN_REQUEST);

                if (m_Client.hasProfilePicture())
                    m_ChatLayer->getChatWindow().addUserProfilePictureTextureID(
                        username, m_Renderer.getClientSideProfilePictureTextureID()
                    );
            });

        m_LoginLayer->getClientLoginPanel().setOnRegisterClick([this]()
        {
            m_CurrentLayer = m_RegisterLayer;
        });

        m_LoginLayer->getClientLoginPanel().setOnLoadProfilePicture([this]()
        {
            std::thread([this]()
            {
                m_FileExplorer.open();
            }).detach();
        });

        m_RegisterLayer = std::make_shared<client::ui::layer::UIRegisterLayer>();

        m_RegisterLayer->getClientRegisterPanel().setOnRegisterDataInput(
            [this](std::string_view username, std::string_view password, std::string_view serverPort)
            {
                onConnect(username, password, serverPort, protocol::Command::REGISTER_REQUEST);

                if (m_Client.hasProfilePicture())
                    m_ChatLayer->getChatWindow().addUserProfilePictureTextureID(
                        username, m_Renderer.getClientSideProfilePictureTextureID()
                    );
            });

        m_RegisterLayer->getClientRegisterPanel().setOnLoginClick([this]()
        {
            m_CurrentLayer = m_LoginLayer;
        });

        m_RegisterLayer->getClientRegisterPanel().setOnLoadProfilePicture([this]()
        {
            std::thread([this]()
            {
                m_FileExplorer.open();
            }).detach();
        });

        m_CurrentLayer = m_LoginLayer;
    }

    void ClientApplication::initSounds()
    {
        auto result = m_SendPressSound.load("assets/sounds/send_button_press_sound.wav");

        if (!result)
            m_ChatLayer->getInfoPanel().addFailInfo(result.error());
    }

    void ClientApplication::onConnect(std::string_view username, std::string_view password, std::string_view serverPort,
                                      protocol::Command joinType)
    {
        m_Client.setUsername(username);

        m_Client.setPassword(password);

        auto result = m_Client.connect("127.0.0.1", serverPort, joinType);

        if (!result)
            m_ChatLayer->getInfoPanel().addFailInfo(result.error());
    }

    void ClientApplication::handleProfilePictureCreation(std::string_view username,
                                                         const profile_picture::ProfilePicture &profilePicture)
    {
        m_Renderer.createProfilePicture(username, profilePicture);

        m_ChatLayer->getChatWindow().addUserProfilePictureTextureID(
            username, m_Renderer.getProfilePictureTextureID(username));
    }
}
