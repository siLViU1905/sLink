#include "ClientApplication.h"

namespace sLink::client_application
{
    ClientApplication::ClientApplication(int windowWidth, int windowHeight,
                                         std::string_view windowName) : application::Application(
                                                                            windowWidth, windowHeight, windowName),
                                                                        m_Client(m_IOContext)
    {
        initLayers();

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

                default:
                    break;
            }
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
            });

        m_LoginLayer->getClientLoginPanel().setOnRegisterClick([this]()
        {
            m_CurrentLayer = m_RegisterLayer;
        });

        m_RegisterLayer = std::make_shared<client::ui::layer::UIRegisterLayer>();

        m_RegisterLayer->getClientRegisterPanel().setOnRegisterDataInput(
            [this](std::string_view username, std::string_view password, std::string_view serverPort)
            {
                onConnect(username, password, serverPort, protocol::Command::REGISTER_REQUEST);
            });

        m_RegisterLayer->getClientRegisterPanel().setOnLoginClick([this]()
        {
            m_CurrentLayer = m_LoginLayer;
        });

        m_CurrentLayer = m_LoginLayer;
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
}
